#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include <QButtonGroup>
#include <QVector>
#include <QDate>
#include <QCheckBox>
#include <QHeaderView>
#include <QListWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <QComboBox>
#include <QFile>       // allow Qt to find file (style.qss)
#include <QTextStream> // allow reading stream of text from style.qss
#include <QDebug>      // print warning if external qss file can't be found
#include <QDateEdit>   // QT date picker object
#include <stdexcept>  // standard C++ exceptions lib


/**
 * @class Assignment
 * @brief Abstract base class representing a generic school assignment
 *
 * Demonstrates encapsulation by keeping member variables private
 * forcing external code to use public getter/setter methods.
 * Demonstrates abstraction by including pure virtual functions
 */
class Assignment {
private:
    QString m_title;
    QDate m_dueDate;
    bool m_isCompleted;

public:
    /**
     * @brief Constructor for the base assignment class
     */
    Assignment(QString &title, QDate &dueDate, bool isCompleted = false) {
        m_title = title;
        m_dueDate = dueDate;
        m_isCompleted = isCompleted;
    }

    /**
     * @brief Virtual Destructor
     * Ensures when child object (such as Exam #1) is
     * deleted via a base pointer, the child's destructor
     * is called first --> preventing memory leaks
     */
    virtual ~Assignment() {}

    // getters & setters
    QString getTitle() const {
        return m_title;
    }

    QDate getDueDate() const {
        return m_dueDate;
    }

    bool isCompleted() const {
        return m_isCompleted;
    }

    void setCompleted(bool status) {
        m_isCompleted = status;
    }

    /**
     *  @brief Pure virtual function for polymorphic behavior
     *  @return A formatted string detailing specific assignment type
     *
     *  Set to "=0" so a raw 'Assignment' object cannot be
     *  instantiated.
     *  Every child class has to write their own version of this function.
     *
     */
    virtual QString getDetails() const = 0;
};

/**
 * @class Exam
 * @brief Derived class (1) representing a test or exam
 *
 * Demonstrates inheritance via inheriting from Assignment class.
 * Demonstrates runtime polymorphism via an override
 * of the getDetails method.
 */
class Exam : public Assignment {
private:
    int m_minuteDuration;

public:
    Exam(QString title, QDate dueDate, int minuteDuration)
        : Assignment(title, dueDate), m_minuteDuration(minuteDuration) {}

    /**
     * @brief Polymorphic override that formats the child class's
     * unique duration values for the UI table
     */
    QString getDetails() const override {
        return QString("Exam: %1 Minutes").arg(m_minuteDuration);
    }
};

/**
 * @class Homework
 * @brief Derived class (2) that represents a standard
 *        homework assignment
 */
class Homework : public Assignment {
private:
    int m_tasks;
public:
    Homework(QString title, QDate dueDate, int tasks)
        : Assignment(title, dueDate), m_tasks(tasks) {}

    /**
     * @brief Formats the child class's unique task values for the UI table
     */
    QString getDetails() const override {
        return QString("Homework: %1 Tasks").arg(m_tasks);
    }
};

/**
 * @class Course
 * @brief Represents a single school course with its own assignments
 *
 * A course physically 'owns' multiple assignments and is
 * responsible for managing their memory.
 */
class Course {
private:
    QString m_name;

    /**
     *  @brief A dynamic list of Qt's implementation of std::vector.
     *         We hold the pointer to the individual Assignments to
     *         be able to mix different classes of assignments inside
     *         one vector.
     *
     *         Recall: vectors can only hold values of the same kind of type.
     *         Because we only store the pointer of a given assignment,
     *         different child objects assignments can be stored together.
     *
     *         Since Assignment is an Abstract class containing a pure virtual
     *         function, its objects can't be stored by value - they must
     *         be stored as pointers.
     */
    QVector<Assignment*> m_assignments;

public:
    Course(QString name) : m_name(name) {}

    /**
     *  @brief Destructor to clean up memory when the course object
     *         is deleted.
     *         Prevents memory leaks by manually deleting all child
     *         Assignment pointers
     */
    ~Course() {
        for (Assignment* a : m_assignments) {
            delete a;
        }
    }

    // --- Course Operations ---
    QString getName() const {
        return m_name;
    }

    /**
     * @brief Returns list of assignments for a given course.
     *
     *         The '&' in the return type means this function returns by
     *         Reference.
     *         Instead of creating a clone of the entire vector, we can
     *         access the original m_assignments vector.
     *         This ensures when we add/remove an assignment, we modify the
     *         actual course object's data rather than modifying a copy.
     */
    QVector<Assignment*>& getAssignments() {
        return m_assignments;
    }

    /**
     *
     * @brief Adds a new assignment to the end of the course's list.
     *        A pointer (a) to a given Assignment object is handed over
     *        to the Course.
     *        The course then takes ownership of its memory & guarantees
     *        it will be safely deleted by its destructor.
     */
    void addAssignment(Assignment* a) {
        m_assignments.push_back(a);
    }
    void dropAssignment(int index) {
        if (index >= 0 && index < m_assignments.size()) {
            // remove allocated memory
            delete m_assignments[index];
            // remove pointer from vector of assignments
            m_assignments.removeAt(index);
        }
    }
};

/**
 * @class MainWindow
 * @brief Core UI class that builds the GUI & handles events.
 *
 *         Inherits all standard window behaviors from QMainWindow
 *         class.
 */
class MainWindow : public QMainWindow {

private:
    // --- Core Data State --
    // master list of all created courses
    QVector<Course*> m_courses;
    // pointer tracking course being views by user
    Course* m_currentCourse = nullptr;

    // UI elements to be accessed across helper functions
    QStackedWidget *m_contentStack;
    QListWidget *m_courseList;
    QLineEdit *m_courseInput;
    QLabel *m_courseTitleLabel;
    QTableWidget *m_assignmentTable;
    QLineEdit *m_assignmentInput;
    QComboBox *m_assignmentTypeCombo;
    QDateEdit *m_assignmentDate;
    QPushButton *m_btnAddAssignment;
    QPushButton *m_btnDropAssignment;


public:
    /**
     *
     * @brief Main window constructor - builds entire UI layout on launch.
     */
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Assignment Tracker");
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

        // page 0 --> dashboard for all courses
        QWidget *pageDashboard = new QWidget(m_contentStack);
        QVBoxLayout *dashboardLayout = new QVBoxLayout(pageDashboard);
        dashboardLayout -> setContentsMargins(30, 30, 30, 30);

        QLabel *dashboardTitle = new QLabel("My Courses", pageDashboard);
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

        dashboardLayout -> addWidget(dashboardTitle);
        dashboardLayout -> addWidget(m_courseList);
        dashboardLayout -> addLayout(addCourseLayout);

        // --- Page 1: Course detail & All Assignments view ---
        QWidget *pageCourseDetail = new QWidget(m_contentStack);
        QVBoxLayout *courseLayout = new QVBoxLayout(pageCourseDetail);
        courseLayout -> setContentsMargins(30, 30, 30, 30);

        QHBoxLayout *courseHeaderLayout = new QHBoxLayout;
        QPushButton *backBtn = new QPushButton(" ← Back", pageCourseDetail);
        backBtn -> setFixedWidth(100);
        m_courseTitleLabel = new QLabel("Course Name", pageCourseDetail);
        m_courseTitleLabel -> setStyleSheet("font-size: 24px; font-weight: bold;");
        courseHeaderLayout -> addWidget(backBtn);
        courseHeaderLayout -> addWidget(m_courseTitleLabel);
        courseHeaderLayout -> addStretch();

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
        m_assignmentInput -> setPlaceholderText("New assignment title: ");

        m_assignmentDate = new QDateEdit(pageCourseDetail);
        m_assignmentDate -> setDate(QDate::currentDate());  // default to current date
        m_assignmentDate -> setCalendarPopup(true);  // allow calendar popup to show
        // more UI-friendly than inputting date as
        // standard MM-DD-YYYY format

        m_btnAddAssignment = new QPushButton("Add Assignment", pageCourseDetail);
        m_btnAddAssignment -> setObjectName("actionBtn");
        m_btnDropAssignment = new QPushButton("Drop Selected Assignment", pageCourseDetail);
        m_btnDropAssignment -> setObjectName("dangerBtn");

        addAssignmentLayout -> addWidget(m_assignmentTypeCombo);
        addAssignmentLayout -> addWidget(m_assignmentInput);
        addAssignmentLayout -> addWidget(m_assignmentDate);
        addAssignmentLayout -> addWidget(m_btnAddAssignment);
        addAssignmentLayout -> addWidget(m_btnDropAssignment);

        courseLayout -> addLayout(courseHeaderLayout);
        courseLayout -> addWidget(m_assignmentTable);
        courseLayout -> addLayout(addAssignmentLayout);

        // --- Page 2: Settings  (Placeholder)
        QLabel *pageSettings = new QLabel("Settings Menu", m_contentStack);
        pageSettings -> setAlignment(Qt::AlignCenter);

        // --- Register pages into the StackedWidget ----
        m_contentStack -> addWidget(pageDashboard);    // index 0
        m_contentStack -> addWidget(pageCourseDetail); // index 1
        m_contentStack -> addWidget(pageSettings);     // index 2

        // Layout assembly
        mainLayout -> addWidget(sidebar);
        mainLayout -> addWidget(m_contentStack);

        // initial dummy data
        Course* software = new Course("Software Testing");
        // set: due date 2wks away from current date
        // expected time = 60min
        software -> addAssignment(new Homework("Assignment 4", QDate::currentDate().addDays(14), 60));
        m_courses.push_back(software);

        Course* math = new Course("Calculus 3");
        math -> addAssignment(new Exam("Midterm Exam", QDate::currentDate().addDays(7), 120));
        m_courses.push_back(math);

        refreshCourseList();

        /**
         * Qt 'Signals' & 'Slots' (event listeners)
         *
         * GUI events (like clicks) emit 'Signals' to be listened to
         * by functions 'Slots'.
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
                refreshAssignmentTable(); // call loop function to refresh table

                // hide controls pertinent to
                // an individual course
                m_assignmentTypeCombo -> hide();
                m_assignmentInput -> hide();
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

        // Event: Course item clicked in Dashboard list
        connect(m_courseList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            int index = m_courseList -> row(item);
            if (index >= 0 && index < m_courses.size()) {
                m_currentCourse = m_courses[index]; // Set active context
                m_courseTitleLabel -> setText(m_currentCourse -> getName());
                refreshAssignmentTable();

                // show controls pertinent to a course
                // if a specified course is now active
                m_assignmentTypeCombo -> show();
                m_assignmentInput -> show();
                m_assignmentDate -> show();
                m_btnAddAssignment -> show();
                m_btnDropAssignment -> show();

                m_contentStack -> setCurrentIndex(1);   // return to view of given course
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
                   m_currentCourse -> addAssignment(new Homework(title, selectedDate, 1));
               } else {
                   m_currentCourse -> addAssignment(new Exam(title, selectedDate, 90));
               }
               m_assignmentInput -> clear();
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

        // load QSS file externally
        QFile styleFile("../style.qss");
        try {
            // check if file exists, can be read & is of type text
            // bitwise OR flag passed in to make sure both conditions are met
            if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
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
    void refreshCourseList() {
        m_courseList -> clear();
        for (Course* c : m_courses) {
            m_courseList -> addItem(c -> getName());
        }
    }

    /**
     * @brief Aggregates data from all courses into single view
     *        Used by 'All Assignments' sidebar tab
     */
    void returnAllAssignments() {
        m_courseTitleLabel -> setText("All Active Assignments");
        m_assignmentTable -> setRowCount(0);

        for (Course* course : m_courses) {
            QVector<Assignment*>& assignments = course -> getAssignments();
            // Nested loop: iterate over every course, then every assignment inside it
            for (Assignment* current : assignments) {
                int row = m_assignmentTable -> rowCount();
                m_assignmentTable -> insertRow(row);

                // Set up interactive checkbox
                QWidget *checkBoxWidget = new QWidget();
                QHBoxLayout *checkBoxLayout = new QHBoxLayout(checkBoxWidget);
                QCheckBox *checkBox = new QCheckBox;
                checkBox -> setChecked(current -> isCompleted());
                checkBoxLayout -> addWidget(checkBox);
                checkBoxLayout -> setAlignment(Qt::AlignCenter);
                checkBoxLayout -> setContentsMargins(0, 0, 0, 0);
                m_assignmentTable -> setCellWidget(row, 0, checkBoxWidget);

                connect(checkBox, &QCheckBox::toggled, this, [current](bool checked) {
                    current -> setCompleted(checked); // update underlying object
                });

                m_assignmentTable -> setItem(row, 1, new QTableWidgetItem(current -> getTitle()));
                m_assignmentTable -> setItem(row, 2, new QTableWidgetItem(current -> getDueDate().toString("MM/dd/yyyy")));

                // Add course name to assignment details string so user can
                // easily ID which course the assignment belongs to in the
                // aggregate "All Assignments" view
                QString courseDetailString = QString("[%1] %2").arg(course -> getName(), current -> getDetails());
                m_assignmentTable -> setItem(row, 3, new QTableWidgetItem(courseDetailString));
            }
        }
    }

    /*
     * @brief Redraws the assignment table only for
     *        the currently selected course.
     */
    void refreshAssignmentTable() {
        // exit out of function if no course is selected
        if (!m_currentCourse) return;
        // clear table
        m_assignmentTable -> setRowCount(0);
        QVector<Assignment*>& assignments = m_currentCourse -> getAssignments();

        for (int row = 0; row < assignments.size(); ++row) {
            m_assignmentTable -> insertRow(row);
            Assignment* current = assignments[row];

            // Set up checkbox to mark status
            QWidget *checkBoxWidget = new QWidget();
            QHBoxLayout *checkBoxLayout = new QHBoxLayout(checkBoxWidget);
            QCheckBox *checkBox = new QCheckBox();
            checkBox -> setChecked(current -> isCompleted());
            checkBoxLayout -> addWidget(checkBox);
            checkBoxLayout -> setAlignment(Qt::AlignCenter);
            checkBoxLayout -> setContentsMargins(0, 0, 0, 0);
            m_assignmentTable -> setCellWidget(row, 0, checkBoxWidget);

            // connect checkbox toggle so it updates underlying object
            connect(checkBox, &QCheckBox::toggled, this, [current](bool checked) {
                current -> setCompleted(checked);
            });

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
    ~MainWindow() {
        for (Course* c : m_courses) {
            delete c;
        }
    }
};

/**
 * @brief Main entry point of the application.
 * @param argc The number of command line arguments.
 *             Although no CLI-args are passed by the user
 *             to the app, the OS always passes at least 1 -
 *             the name of the executable/executable path.
 * @param argv The array of command line arguments.
 *             Handed directly over to QApplication so Qt can
 *             automatically handle OS-level paths & built-in
 *             Qt flags.
 * @return An integer exist code (0 for successful execution)
 *
 *         This function initializes the Qt GUI framework, builds the
 *         main window and enters the event loop (app.exec()) so the
 *         application stays open & responds to user input.
 *
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show(); // display the UI
    return app.exec(); // enter event loop
}
