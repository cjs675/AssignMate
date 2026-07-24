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
#include <QTextStream> // allow read of stream of text from style.qss
#include <QDebug>      // print warning if external qss file can't be found
#include <QDateEdit>   // QT date picker object



// BASE class
class Assignment {
private:
    QString m_title;
    QDate m_dueDate;
    bool m_isCompleted;


public:
    Assignment(QString title, QDate dueDate, bool isCompleted = false) {
        m_title = title;
        m_dueDate = dueDate;
        m_isCompleted = isCompleted;
    }

    // destructor marked as virtual to ensure cleanup
    // of proper object
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

    // marked as virtual to return details of
    // respective Assignment object/child classes
    // such as Exam, Homework, etc
    // set to = 0 so every child class is its own
    virtual QString getDetails() const = 0;
};

// DERIVED class (1)
class Exam : public Assignment {
private:
    int m_minuteDuration;

public:
    Exam(QString title, QDate dueDate, int minuteDuration)
        : Assignment(title, dueDate), m_minuteDuration(minuteDuration) {}

    // polymorphic override
    // formats child class's unique duration values for the UI table
    QString getDetails() const override {
        return QString("Exam: %1 Minutes").arg(m_minuteDuration);
    }
};

// DERIVED class (2)
class Homework : public Assignment {
private:
    int m_tasks;
public:
    Homework(QString title, QDate dueDate, int tasks)
        : Assignment(title, dueDate), m_tasks(tasks) {}
    // polymorphic override
    // formats child class's unique duration values for the UI table
    QString getDetails() const override {
        return QString("Homework: %1 Tasks").arg(m_tasks);
    }
};

class Course {
private:
    QString m_name;
    QVector<Assignment*> m_assignments;

public:
    Course(QString name) : m_name(name) {}

    // destructor to clean up memory when
    // course object is deleted
    ~Course() {
        for (Assignment* a : m_assignments) {
            delete a;
        }
    }

    QString getName() const {
        return m_name;
    }
    QVector<Assignment*>& getAssignments() {
        return m_assignments;
    }
    void addAssignment(Assignment* a) {
        m_assignments.push_back(a);
    }
    void dropAssignment(int index) {
        if (index >= 0 && index < m_assignments.size()) {
            delete m_assignments[index];
            m_assignments.removeAt(index);
        }
    }
};

// window class that populates GUI elements
class MainWindow : public QMainWindow {

private:
    // core data state
    QVector<Course*> m_courses;
    Course* m_currentCourse = nullptr;

    // UI elements to be accessed across functions
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

        // page 1 --> course xyz view
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

        // add/drop controls for assignments
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

        // page 2 --> settings
        QLabel *pageSettings = new QLabel("Settings Menu", m_contentStack);
        pageSettings -> setAlignment(Qt::AlignCenter);

        m_contentStack -> addWidget(pageDashboard);    // index 0
        m_contentStack -> addWidget(pageCourseDetail); // index 1
        m_contentStack -> addWidget(pageSettings);     // index 2

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

        // Qt 'Signals' & 'Slots'
        // aid in performing actions based on specific input

        // sidebar navigation
        connect(navGroup, &QButtonGroup::idClicked, this, [this](int id) {
            // map sidebar index
            if (id == 0) m_contentStack -> setCurrentIndex(0);
            if (id == 1) m_contentStack -> setCurrentIndex(1);
            if (id == 2) m_contentStack -> setCurrentIndex(2);
        });

        // dashboard: add course
        connect(btnAddCourse, &QPushButton::clicked, this, [this]() {
           if (!m_courseInput -> text().isEmpty()) {
               m_courses.push_back(new Course(m_courseInput -> text()));
               m_courseInput -> clear();
               refreshCourseList();
           }
        });

        // receive signal of course clicked --> enter course page
        connect(m_courseList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            int index = m_courseList -> row(item);
            if (index >= 0 && index < m_courses.size()) {
                m_currentCourse = m_courses[index];
                m_courseTitleLabel -> setText(m_currentCourse -> getName());
                refreshAssignmentTable();
                m_contentStack -> setCurrentIndex(1);   // return to view of given course
            }
        });

        // receive signal: back button click
        connect(backBtn, &QPushButton::clicked, this, [this]() {
            m_currentCourse = nullptr;
            m_contentStack -> setCurrentIndex(0);
        });

        // receive signal: add assignment button click
        connect(btnAddAssignment, &QPushButton::clicked, this, [this]() {
           if (m_currentCourse && !m_assignmentInput -> text().isEmpty()) {
               QString title = m_assignmentInput -> text();

               QDate selectedDate = m_assignmentDate -> date();

               if (m_assignmentTypeCombo -> currentText() == "Homework") {
                   m_currentCourse -> addAssignment(new Homework(title, selectedDate, 1));
               } else {
                   m_currentCourse -> addAssignment(new Exam(title, selectedDate, 90));
               }
               m_assignmentInput -> clear();
               refreshAssignmentTable();
           }
        });

        // receive signal to 'drop' assignment
        connect(btnDropAssignment, &QPushButton::clicked, this, [this]() {
           if (m_currentCourse && m_assignmentTable -> currentRow() >= 0) {
               m_currentCourse -> dropAssignment(m_assignmentTable -> currentRow());
               refreshAssignmentTable();
           }
        });

        // load QSS file externally
        QFile styleFile("../style.qss");
        // check if file exists, can be read & is of type text
        // bitwise OR flag passed in to make sure both conditions are met
        if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
            // style.qss passed by reference into stream
           QTextStream stream(&styleFile);
            // set style of main window to stream of QSS text read from style.qss
            this -> setStyleSheet(stream.readAll());
            // close file after reading so it doesn't stay open in memory
            styleFile.close();
        } else {
            // message to print if style file cannot be found
            qDebug() << "Error: style.qss not found...";
        }
    }

    // helper function to redraw list of courses on main dashboard
    void refreshCourseList() {
        m_courseList -> clear();
        for (Course* c : m_courses) {
            m_courseList -> addItem(c -> getName());
        }
    }

    // helper function to redraw the assignment table
    // for the current course
    void refreshAssignmentTable() {
        if (!m_currentCourse) return;
        // clear table
        m_assignmentTable -> setRowCount(0);
        QVector<Assignment*>& assignments = m_currentCourse -> getAssignments();

        for (int row = 0; row < assignments.size(); ++row) {
            m_assignmentTable -> insertRow(row);
            Assignment* current = assignments[row];

            // checkbox to mark status
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

    // destructor that cleans up course objects
    // stored within course vector
    ~MainWindow() {
        for (Course* c : m_courses) {
            delete c;
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();
    return app.exec();
}
