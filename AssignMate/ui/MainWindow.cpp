//
// Created by Chris Silva on 8/5/26.
//

#include "MainWindow.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include <QButtonGroup>
#include <QDate>
#include <QHeaderView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateEdit>
#include <QPixmap>
#include <QAbstractItemView>
#include <stdexcept>

#include "Exam.h"
#include "Homework.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
        setWindowTitle("AssignMate");
        resize(1000, 600);

        // 1. core container layout (main window)
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        QHBoxLayout *mainLayout= new QHBoxLayout(centralWidget);
        mainLayout -> setContentsMargins(0, 0, 0, 0); // remove default padding
        mainLayout -> setSpacing(0);

        // 2. sidebar setup
        QWidget *sidebar = new QWidget(this);
        sidebar -> setObjectName("sidebarWidget"); // tagged for QSS styling
        sidebar -> setFixedWidth(220);
        QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
        sidebarLayout -> setContentsMargins(0, 20, 0, 20);
        sidebarLayout -> setSpacing(5);

        // create nav buttons
        QPushButton *btnDashboard = new QPushButton("Dashboard", sidebar);
        QPushButton *btnAssignments = new QPushButton("All Assignments", sidebar);
        QPushButton *btnSettings = new QPushButton("Settings", sidebar);

        // make buttons checkable for "active" styling state
        btnDashboard -> setCheckable(true);
        btnAssignments -> setCheckable(true);
        btnSettings -> setCheckable(true);
        btnDashboard -> setChecked(true);   // default to main dashboard

        // group buttons so only one can be checked at a time
        QButtonGroup *navGroup = new QButtonGroup(this);
        navGroup -> setExclusive(true);
        navGroup -> addButton(btnDashboard, 0);
        navGroup -> addButton(btnAssignments, 1);
        navGroup -> addButton(btnSettings, 2);

        // add buttons to sidebar, then push to top with spacer
        sidebarLayout -> addWidget(btnDashboard);
        sidebarLayout -> addWidget(btnAssignments);
        sidebarLayout -> addWidget(btnSettings);
        sidebarLayout -> addStretch();

        m_contentStack = new QStackedWidget(this);

        // -- Shared top-right content panel with persistent logo --
        QWidget *rightPanel = new QWidget(this);
        QVBoxLayout *rightPanelLayout = new QVBoxLayout(rightPanel);
        rightPanelLayout -> setContentsMargins(0, 0, 0, 0);
        rightPanelLayout -> setSpacing(0);

        QHBoxLayout *globalTopBar = new QHBoxLayout();
        globalTopBar -> setContentsMargins(30, 20, 30, 0);

        QLabel *logoLabel = new QLabel(rightPanel);
        QPixmap logo(":/logo.png");
        logoLabel -> setPixmap(
            logo.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );

        globalTopBar -> addStretch();
        globalTopBar -> addWidget(logoLabel);
        rightPanelLayout -> addLayout(globalTopBar);
        // Wrap m_contentStack in a right-side panel so logo
        // remains visible across all pages
        rightPanelLayout -> addWidget(m_contentStack);

        // page 0 --> dashboard for all courses
        QWidget *pageDashboard = new QWidget(m_contentStack);
        QVBoxLayout *dashboardLayout = new QVBoxLayout(pageDashboard);
        dashboardLayout -> setContentsMargins(30, 30, 30, 30);

        QLabel *dashboardTitle = new QLabel("My Courses", pageDashboard);
        dashboardTitle -> setAlignment(Qt::AlignCenter);
        dashboardTitle -> setStyleSheet("font-size: 24px; "
                                       "font-weight: bold;"
                                       " margin-bottom: 10px");
        m_courseList = new QListWidget(pageDashboard);
        m_courseList -> setObjectName("courseList");

        QHBoxLayout *addCourseLayout = new QHBoxLayout();
        m_courseInput = new QLineEdit(pageDashboard);
        m_courseInput -> setPlaceholderText("Enter course name: ");
        QPushButton *btnAddCourse = new QPushButton("Add Course", pageDashboard);
        btnAddCourse -> setObjectName("actionBtn");

        addCourseLayout -> addWidget(m_courseInput);
        addCourseLayout -> addWidget(btnAddCourse);

        // Layout for course management buttons
        QHBoxLayout *courseManagementLayout = new QHBoxLayout();
        QPushButton *btnOpenCourse = new QPushButton("Open Course", pageDashboard);
        btnOpenCourse -> setObjectName("actionBtn");
        QPushButton *btnDropCourse = new QPushButton("Drop Course", pageDashboard);
        btnDropCourse -> setObjectName("dangerBtn");
        courseManagementLayout -> addStretch();
        courseManagementLayout -> addWidget(btnOpenCourse);
        courseManagementLayout -> addWidget(btnDropCourse);

        dashboardLayout -> addWidget(dashboardTitle);
        dashboardLayout -> addWidget(m_courseList);
        dashboardLayout -> addLayout(addCourseLayout);
        dashboardLayout -> addLayout(courseManagementLayout);

        // --- Page 1: Course detail & All Assignments view ---
        QWidget *pageCourseDetail = new QWidget(m_contentStack);
        QVBoxLayout *courseLayout = new QVBoxLayout(pageCourseDetail);
        QWidget *rightHeaderSpacer = new QWidget(pageCourseDetail);
        rightHeaderSpacer -> setFixedWidth(100);

        courseLayout -> setContentsMargins(30, 30, 30, 30);

        QHBoxLayout *courseHeaderLayout = new QHBoxLayout;
        QPushButton *backBtn = new QPushButton(" ← Back", pageCourseDetail);
        backBtn -> setFixedWidth(100);
        m_courseTitleLabel = new QLabel("Course Name", pageCourseDetail);
        m_courseTitleLabel -> setStyleSheet("font-size: 24px; font-weight: bold;");
        m_courseTitleLabel -> setAlignment(Qt::AlignCenter);

        courseHeaderLayout -> addWidget(backBtn);
        courseHeaderLayout -> addStretch();
        courseHeaderLayout -> addWidget(m_courseTitleLabel);
        courseHeaderLayout -> addStretch();
        courseHeaderLayout -> addWidget(rightHeaderSpacer);

        // Data table setup
        m_assignmentTable = new QTableWidget(0, 4, pageCourseDetail);
        m_assignmentTable -> setHorizontalHeaderLabels({ "Status",
                                                            "Title",
                                                            "Due Date",
                                                            "Details "});
        m_assignmentTable -> horizontalHeader() -> setSectionResizeMode(QHeaderView::Stretch);
        m_assignmentTable -> horizontalHeader() -> setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_assignmentTable -> verticalHeader() -> setVisible(false);
        m_assignmentTable -> setSelectionBehavior(QAbstractItemView::SelectRows);
        m_assignmentTable -> setShowGrid(false);

        // Add/Drop controls for assignments
        QHBoxLayout *addAssignmentLayout = new QHBoxLayout();
        m_assignmentTypeCombo = new QComboBox(pageCourseDetail);
        m_assignmentTypeCombo -> addItems({"Homework", "Exam"});
        m_assignmentInput= new QLineEdit(pageCourseDetail);
        m_assignmentInput -> setPlaceholderText("Title: ");
        m_assignmentTopicInput = new QLineEdit(pageCourseDetail);
        m_assignmentTopicInput -> setPlaceholderText("Topic: ");

        m_assignmentDate = new QDateEdit(pageCourseDetail);
        m_assignmentDate -> setDate(QDate::currentDate());  // default to current date
        m_assignmentDate -> setCalendarPopup(true);  // allow calendar popup to show
        // more UI-friendly than inputting date as
        // standard MM-DD-YYYY format

        m_btnAddAssignment = new QPushButton("Add\n Assignment", pageCourseDetail);
        m_btnAddAssignment -> setObjectName("actionBtn");
        m_btnDropAssignment = new QPushButton("Drop\n Assignment", pageCourseDetail);
        m_btnDropAssignment -> setObjectName("dangerBtn");

        m_btnMarkComplete = new QPushButton("Mark Complete", pageCourseDetail);
        m_btnMarkComplete -> setObjectName("completeBtn");
        m_btnMarkComplete -> setFixedWidth(140);
        m_btnMarkIncomplete = new QPushButton("Mark Incomplete", pageCourseDetail);
        m_btnMarkIncomplete -> setObjectName("incompleteBtn");
        m_btnMarkIncomplete-> setFixedWidth(140);

        addAssignmentLayout -> addWidget(m_assignmentTypeCombo);
        addAssignmentLayout -> addWidget(m_assignmentInput);
        addAssignmentLayout -> addWidget(m_assignmentTopicInput);
        addAssignmentLayout -> addWidget(m_assignmentDate);
        addAssignmentLayout -> addWidget(m_btnAddAssignment);
        addAssignmentLayout -> addWidget(m_btnDropAssignment);
        addAssignmentLayout -> addStretch();
        addAssignmentLayout -> addWidget(m_btnMarkComplete);
        addAssignmentLayout -> addWidget(m_btnMarkIncomplete);

        courseLayout -> addLayout(courseHeaderLayout);
        courseLayout -> addWidget(m_assignmentTable);
        courseLayout -> addLayout(addAssignmentLayout);

        // --- Page 2: Settings
        QWidget *pageSettings = new QWidget(m_contentStack);
        QVBoxLayout *settingsLayout = new QVBoxLayout(pageSettings);
        settingsLayout -> setContentsMargins(30, 30, 30, 30);
        settingsLayout -> setSpacing(15);

        QLabel *settingsTitle = new QLabel("Settings", pageSettings);
        settingsTitle -> setAlignment(Qt::AlignCenter);
        settingsTitle -> setStyleSheet("font-size: 22px; font-weight: bold; color: #2979ff;");

        QLabel *appNameLabel = new QLabel("AssignMate", pageSettings);
        appNameLabel -> setAlignment(Qt::AlignCenter);
        appNameLabel -> setStyleSheet("font-size: 22px; font-weight: bold; color: #2979ff;");

        QLabel *appDescriptionLabel = new QLabel("A simple assignment tracker for managing course workload");
        appDescriptionLabel -> setAlignment(Qt::AlignCenter);
        appDescriptionLabel -> setWordWrap(true);
        appDescriptionLabel -> setStyleSheet("font-size: 14px; color: #8a99af;");

        QLabel *builtWithLabel = new QLabel("Built with Qt, C++17 & CMake", pageSettings);
        builtWithLabel -> setAlignment(Qt::AlignCenter);
        builtWithLabel -> setStyleSheet("font-size: 14px; color: #8a99af;");

        QLabel *versionLabel = new QLabel("Version 1.0", pageSettings);
        versionLabel -> setAlignment(Qt::AlignCenter);
        versionLabel -> setStyleSheet("font-size: 14px; color: #8a99af;");

        settingsLayout -> addStretch();
        settingsLayout -> addWidget(settingsTitle);
        settingsLayout -> addSpacing(20);
        settingsLayout -> addWidget(appNameLabel);
        settingsLayout -> addWidget(appDescriptionLabel);
        settingsLayout -> addWidget(builtWithLabel);
        settingsLayout -> addWidget(versionLabel);
        settingsLayout -> addStretch();

        // --- Register pages into the StackedWidget ----
        m_contentStack -> addWidget(pageDashboard);    // index 0
        m_contentStack -> addWidget(pageCourseDetail); // index 1
        m_contentStack -> addWidget(pageSettings);     // index 2

        // Layout assembly
        mainLayout -> addWidget(sidebar);
        mainLayout -> addWidget(rightPanel);

        // initial dummy data
        Course* testing = new Course("Software Testing");
        // set: due date 2wks away from current date
        // Topic: Unit Tests
        testing -> addAssignment(new Homework("Assignment 1", QDate::currentDate().addDays(14), "Unit Tests"));
        m_courses.push_back(testing);

        Course* math = new Course("Calculus 3");
        math -> addAssignment(new Exam("Midterm Exam", QDate::currentDate().addDays(7), 120));
        m_courses.push_back(math);

        Course*  sysAdmin = new Course("Sys Admin & Programming");
        sysAdmin -> addAssignment(new Homework("Lab 1", QDate::currentDate().addDays(14), "Bash Scripts"));
        m_courses.push_back(sysAdmin);

        refreshCourseList();

     /**
     * Qt 'Signals' & 'Slots' (event listeners)
     *
     * GUI events (like clicks) emit 'Signals' to be listened to
     * by functions aka 'Slots'.
     *
     * Breakdown of Qt-style lambda functions (anonymous, inline functions)
     * - [this]   : grants this inline function permission to access MainWindow's
     *              private member variables (like m_contentStack, m_currentCourse)
     * - (int id) : The arameter passed by 'idClicked' signal.
     *              Tells the lambda exactly which button ID was pressed.
     */

    // Event sidebar navigation
    connect(navGroup, &QButtonGroup::idClicked, this, [this](int id) {
        // map sidebar index
        if (id == 0) {
            m_contentStack -> setCurrentIndex(0); // Go to dashboard
        }
        if (id == 1) {
            // Initialize current course context to null (Aggregate view)
            // i.e., no specific course is selected
            m_currentCourse = nullptr;
            returnAllAssignments(); // call loop function to refresh table

            // hide controls pertinent to
            // an individual course
            m_assignmentTypeCombo -> hide();
            m_assignmentInput -> hide();
            m_assignmentTopicInput -> hide();
            m_assignmentDate -> hide();
            m_btnAddAssignment -> hide();
            m_btnDropAssignment -> hide();

            m_contentStack -> setCurrentIndex(1); // Go to table view
        }
        if (id == 2) m_contentStack -> setCurrentIndex(2); // Go to settings
    });

    // Event: 'Add Course' button clicked
    connect(btnAddCourse, &QPushButton::clicked, this, [this]() {
       if (!m_courseInput -> text().isEmpty()) {
           m_courses.push_back(new Course(m_courseInput -> text()));
           m_courseInput -> clear();
           refreshCourseList();
       }
    });

    // Event: 'Drop Course' button clicked
    connect(btnDropCourse, &QPushButton::clicked, this, [this]() {
        int currentIdx = m_courseList -> currentRow();
        // Ensure a valid item is selected
        if (currentIdx >= 0 && currentIdx < m_courses.size()) {
            // Get a pointer to the course object to be deleted
            Course* courseToDelete = m_courses[currentIdx];

            // Remove pointer from master list
            m_courses.removeAt(currentIdx);
            // Delete course object to free its memory
            // ~Course() handles deleting child assignments
            delete courseToDelete;

            // Update UI to reflect removal
            refreshCourseList();
        }
    });

    // Event: 'Open Course' button clicked
    connect(btnOpenCourse, &QPushButton::clicked, this, [this]() {
        int index = m_courseList -> currentRow();
        if (index >= 0 && index < m_courses.size()) {
            m_currentCourse = m_courses[index]; // Set active context
            m_courseTitleLabel -> setText(m_currentCourse -> getName());
            refreshAssignmentTable();

            // show controls pertinent to a course
            // if a specified course is now active
            m_assignmentTypeCombo -> show();
            m_assignmentInput -> show();
            m_assignmentTopicInput -> show();
            m_assignmentDate -> show();
            m_btnAddAssignment -> show();
            m_btnDropAssignment -> show();

            m_contentStack -> setCurrentIndex(1);   // Return to view of a given course
        }
    });

    // Event: Course item DOUBLE-clicked in Dashboard List
    connect(m_courseList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
       int index = m_courseList -> row(item);
        if (index >= 0 && index < m_courses.size()) {
            m_currentCourse = m_courses[index]; // Set active context
            m_courseTitleLabel -> setText(m_currentCourse -> getName());
            refreshAssignmentTable();

            // Show controls pertinent to a course
            // if a specified course is now active
            m_assignmentTypeCombo -> show();
            m_assignmentInput -> show();
            m_assignmentTopicInput -> show();
            m_assignmentDate -> show();
            m_btnAddAssignment -> show();
            m_btnDropAssignment -> show();

            m_contentStack -> setCurrentIndex(1);   // Return to view of a given course
        }
    });

    // Event: 'Back' button clicked from within a course
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        m_currentCourse = nullptr;
        m_contentStack -> setCurrentIndex(0);
    });

    // Event: 'Add Assignment' button clicked
    connect(m_btnAddAssignment, &QPushButton::clicked, this, [this]() {
       if (m_currentCourse && !m_assignmentInput -> text().isEmpty()) {
           QString title = m_assignmentInput -> text();

           QDate selectedDate = m_assignmentDate -> date();

           // determine which child object to create based on dropdown
           if (m_assignmentTypeCombo -> currentText() == "Homework") {
               QString topic = m_assignmentTopicInput -> text();
               if (topic.isEmpty()) {
                   topic = "General"; // Set default if topic = empty
               }
               m_currentCourse -> addAssignment(new Homework(title, selectedDate, topic));
           } else {
               // 'bool ok' used as a safety check when converting
               // from text string to number in Qt
               bool ok;
               int duration = m_assignmentTopicInput -> text().toInt(&ok);
               if (!ok || duration <= 0) {
                   // Default duration set to 90min if input is invalid
                   duration = 90;
               }
               m_currentCourse -> addAssignment(new Exam(title, selectedDate, duration));
           }
           m_assignmentTopicInput -> clear();
           m_assignmentInput -> clear();
           m_assignmentDate -> setDate(QDate::currentDate());
           refreshAssignmentTable();
       }
    });

    // Event: 'Drop Assignment' button clicked
    connect(m_btnDropAssignment, &QPushButton::clicked, this, [this]() {
       if (m_currentCourse && m_assignmentTable -> currentRow() >= 0) {
           m_currentCourse -> dropAssignment(m_assignmentTable -> currentRow());
           refreshAssignmentTable();
       }
    });

    // Event: Assignment type changed
    // Update placeholder between Exam (Duration (Mins):)
    // and Homework (topic)
    connect(m_assignmentTypeCombo, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (text == "Homework") {
            m_assignmentTopicInput -> setPlaceholderText("Topic: ");
        } else {
            m_assignmentTopicInput -> setPlaceholderText("Duration (Mins): ");
        }
    });

    // Event: 'Mark Complete' button clicked
    connect(m_btnMarkComplete, &QPushButton::clicked, this, [this]() {
        int row = m_assignmentTable -> currentRow();

        if (row >= 0 && row < m_table_assignments.size()) {
            m_table_assignments[row] -> setCompleted(true);

            if (m_currentCourse) {
                refreshAssignmentTable();
            } else {
                returnAllAssignments();
            }
        }
    });

    // Event: 'Mark Incomplete' button clicked
    connect(m_btnMarkIncomplete, &QPushButton::clicked, this, [this]() {
        int row = m_assignmentTable -> currentRow();

        if (row >= 0 && row < m_table_assignments.size()) {
            m_table_assignments[row] -> setCompleted(false);

            if (m_currentCourse) {
                refreshAssignmentTable();
            } else {
                returnAllAssignments();
            }
        }
    });

    // load QSS file externally
    QFile styleFile(":/style.qss");
    try {
        // check if file exists, can be read & is of type text
        // bitwise OR flag passed in to make sure both conditions are met
        if (!styleFile.open(QFile::ReadOnly | QFile::Text)) {
            throw std::runtime_error("Could not find or open style.qss...");
        }
        // style.qss passed by reference into stream
        QTextStream stream(&styleFile);
        // set style of main window to stream of QSS text read from style.qss
        this -> setStyleSheet(stream.readAll());
        // close file after reading so it doesn't stay open in memory (prevent memory leaks)
        styleFile.close();
    } catch (const std::exception& e) {
        // message to print if style file cannot be found
        // e.what() prints the string message returned
        // by the try-catch block
        qDebug() << "Exception caught:"  << e.what();
    }
}

/**
 * @brief Clears & redraws the list of courses on the main dashboard
 */
void MainWindow::refreshCourseList() {
    m_courseList -> clear();

    for (Course* c : m_courses) {
        m_courseList -> addItem(c -> getName());
    }
}


/**
 * @brief Aggregates data from all courses into single view
 *        Used by 'All Assignments' sidebar tab
 */
void MainWindow::returnAllAssignments() {
    m_courseTitleLabel -> setText("All Assignments");
    m_assignmentTable -> setColumnCount(5);
    m_assignmentTable -> setHorizontalHeaderLabels({ "Status",
                                                        "Course",
                                                        "Title",
                                                        "Due Date",
                                                        "Details" });
        // clear table
        m_assignmentTable -> setRowCount(0);

        // Clear vector whenever the table is rebuilt.
        // Assignment pointer is pushed into the vector each time a row is added.
        m_table_assignments.clear();

        for (Course* course : m_courses) {
            QVector<Assignment*>& assignments = course -> getAssignments();
            // Nested loop: iterate over every course, then every assignment inside it
            for (Assignment* current : assignments) {
                int row = m_assignmentTable -> rowCount();
                m_assignmentTable -> insertRow(row);
                // keep table row index matched with respective Assignment pointer
                m_table_assignments.push_back(current);

                // Display completion status as text
                QString statusText = current -> isCompleted() ? "Complete" : "Incomplete";
                QTableWidgetItem *itemStatus = new QTableWidgetItem(statusText);
                itemStatus -> setTextAlignment(Qt::AlignCenter);

                // Display completion status with color indicator
                if (current -> isCompleted()) {
                    itemStatus -> setForeground(QBrush(QColor("#2e7d32")));
                } else {
                    itemStatus -> setForeground(QBrush(QColor("#FF3D4D")));
                }

                m_assignmentTable -> setItem(row, 0, itemStatus);

                m_assignmentTable -> setItem(row, 1, new QTableWidgetItem(course -> getName()));
                m_assignmentTable -> setItem(row, 2, new QTableWidgetItem(current -> getTitle()));
                m_assignmentTable -> setItem(row, 3, new QTableWidgetItem(current -> getDueDate().toString("MM/dd/yyyy")));
                m_assignmentTable -> setItem(row, 4, new QTableWidgetItem(current -> getDetails()));
            }
        }
    }

/*
 * @brief Redraws the assignment table only for
 *        the currently selected course.
 */
void MainWindow::refreshAssignmentTable() {
    // exit out of function if no course is selected
    if (!m_currentCourse) return;
    // reset table layout for individual course view
        m_assignmentTable -> setColumnCount(4);
        m_assignmentTable -> setHorizontalHeaderLabels({ "Status",
                                                            "Title",
                                                            "Due Date",
                                                            "Details"});
        // clear table
        m_assignmentTable -> setRowCount(0);
        m_table_assignments.clear();

        // populate table with assignments for the current course
        QVector<Assignment*>& assignments = m_currentCourse -> getAssignments();

    for (int row = 0; row < assignments.size(); ++row) {
        m_assignmentTable -> insertRow(row);
        Assignment* current = assignments[row];

        // keep table row index matched with respective Assignment pointer
        m_table_assignments.push_back(current);

        // Display completion status as text
        QString statusText = current -> isCompleted() ? "Complete" : "Incomplete";
        QTableWidgetItem *itemStatus = new QTableWidgetItem(statusText);
        itemStatus -> setTextAlignment(Qt::AlignCenter);

        // Display completion status with color indicator
        if (current -> isCompleted()) {
            itemStatus -> setForeground(QBrush(QColor("#2e7d32")));
        } else {
            itemStatus -> setForeground(QBrush(QColor("#FF3D4D")));
        }
        m_assignmentTable -> setItem(row, 0, itemStatus);

        m_assignmentTable -> setItem(row, 1, new QTableWidgetItem(current -> getTitle()));
        m_assignmentTable -> setItem(row, 2, new QTableWidgetItem(current -> getDueDate().toString("MM, dd, yyyy")));
        // dynamically call exam or assignment getDetails() method
        m_assignmentTable -> setItem(row, 3, new QTableWidgetItem(current -> getDetails()));
    }
}
/**
  * @brief Destructor to clean up course objects stored
  *        within the course vector.
  *        Prevents memory leaks upon application exit.
 */
MainWindow::~MainWindow() {
    for (Course* c : m_courses) {
        delete c;
    }
}


