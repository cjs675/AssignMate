#include <QApplication>
#include <QMainWindow>
#include <QWidget> 
#include <QHBoxLayout> 
#include <QVBoxLayout>
#include <QPushButton> 
#include <QStackedWidget> 
#include <QLabel> 
#include <QButtonGroup> 



class MainWindow : public QMainWindow {

public: 
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Assignment Tracker");
        resize(1000, 600);

        // 1. core container layour (main window) 
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
        QPushButton *btnAssignments = new QPushButton("Assignments", sidebar);
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

        // 3. content area setup - the stacked widget 
        QStackedWidget *contentStack = new QStackedWidget(this);

        // create dummy pages for the stack 
        QLabel *pageDashboard = new QLabel("Dashboard Overview (Widgets go here)", contentStack);
        pageDashboard -> setAlignment(Qt::AlignCenter);

        QLabel *pageAssignments = new QLabel("Assignment Tracker (SQL Table goes here)", contentStack);
        pageAssignments -> setAlignment(Qt::AlignCenter); 

        QLabel *pageSettings = new QLabel("Settings Menu", contentStack);
        pageSettings -> setAlignment(Qt::AlignCenter);

        // add pages to stack 
        contentStack -> addWidget(pageDashboard);   // index 0
        contentStack -> addWidget(pageAssignments); // index 1
        contentStack -> addWidget(pageSettings);    // index 2 


        // 4. add sidebar * content stack to Main Layour 
        mainLayout -> addWidget(sidebar);
        mainLayout -> addWidget(contentStack);

        // 5. wire the navi logic 
        // when a button is clicked, change the stacked widget's visible page 
        connect(navGroup, &QButtonGroup::idClicked, this, [contentStack](int id) {
            contentStack -> setCurrentIndex(id);
        }); 

        // 6. apply dark mode qt style sheets (QSS) 
        QString darkTheme = R"(
            QMainWindow, QStackedWidget {
                background-color: #0b1421;
            }
            QLabel {
                color: #ffffff;
                font-size: 18px;
            }
            #sidebarWidget {
                background-color: #131c2d;
                border-right: 1px solid #1f2a3f;
            }
            QPushButton {
                background-color: transparent;
                color: #8a99af;
                text-align: left;
                padding: 12px, 20px;
                border: none;
                border-left: 3px solid transparent;
                font-weight: bold;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #1a2536;
                color: #ffffff;
            }
            QPushButton:checked {
                border-left: 3px solid #2979ff;
                color: #ffffff;
                background-color: #1a2536;
            }
        )";
        this -> setStyleSheet(darkTheme);
    }
}; 

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
