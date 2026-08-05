//
// Created by Chris Silva on 8/5/26.
//

#ifndef ASSIGNMENTTRACKER_MAINWINDOW_H
#define ASSIGNMENTTRACKER_MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

class QStackedWidget;
class QListWidget;
class QLineEdit;
class QLabel;
class QTableWidget;
class QComboBox;
class QDateEdit;
class QPushButton;

#include "Assignment.h"
#include "Course.h"

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
    QVector<Course *> m_courses;

    // maps each visible table row to its underlying Assignment object
    QVector<Assignment *> m_table_assignments;

    // pointer tracking course being viewed by user
    Course *m_currentCourse = nullptr;

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
    QPushButton *m_btnMarkComplete;
    QPushButton *m_btnMarkIncomplete;
    QLineEdit *m_assignmentTopicInput;

public:
    /**
     * @brief Main window constructor - builds entire UI layout on launch
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor to clean up course objects stored within course vector
     */
    ~MainWindow();

private:
    /**
     * @brief Clears & redraws the list of courses on the main dashboard
     */
    void refreshCourseList();

    /**
     * @brief Aggregates data from all courses into a single table view
     */
    void returnAllAssignments();

    /**
     * @brief Redraws the assignment table only for the currently selected course
     */
    void refreshAssignmentTable();
};
#endif //ASSIGNMENTTRACKER_MAINWINDOW_H
