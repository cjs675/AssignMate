//
// Created by Chris Silva on 8/5/26.
//

#ifndef ASSIGNMENTTRACKER_HOMEWORK_H
#define ASSIGNMENTTRACKER_HOMEWORK_H

#include "Assignment.h"

/**
 * @class Homework
 * @brief Derived class that represents a standard
 *        homework assignment
 */
class Homework : public Assignment {
private:
    QString m_topic;

public:
    Homework(const QString &title, const QDate &dueDate, const QString &topic);

    /**
     * @brief Formats the child class's unique task values for the UI table
     */
    QString getDetails() const override;
};

#endif //ASSIGNMENTTRACKER_HOMEWORK_H
