//
// Created by Chris Silva on 8/5/26.
//

#ifndef ASSIGNMENTTRACKER_ASSIGNMENT_H
#define ASSIGNMENTTRACKER_ASSIGNMENT_H

#include <QString>
#include <QDate>

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
    QString m_assignmentTopicInput;

public:
    /**
     * @brief Constructor for the base assignment class
     */
    Assignment(const QString &title,
               const QDate &dueDate,
               bool isCompleted = false,
               const QString& assignmentTopicInput = "");

    /**
     * @brief Virtual Destructor
     * Ensures when a child object (such as Exam #1) is
     * deleted via a base pointer, the child's destructor
     * is called first --> preventing memory leaks
     */
    virtual ~Assignment();

    // getters & setters
    QString getTitle() const;
    QDate getDueDate() const;
    bool isCompleted() const;
    void setCompleted(bool status);

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


#endif //ASSIGNMENTTRACKER_ASSIGNMENT_H
