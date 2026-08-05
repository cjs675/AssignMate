//
// Created by Chris Silva on 8/5/26.
//

#ifndef ASSIGNMENTTRACKER_EXAM_H
#define ASSIGNMENTTRACKER_EXAM_H

#include "Assignment.h"

/**
 * @class Exam
 * @brief Derived class representing a test or exam
 *
 * Demonstrates inheritance via inheriting from Assignment class.
 * Demonstrates runtime polymorphism via an override
 * of the getDetails method.
 */
class Exam : public Assignment {
private:
    int m_minuteDuration;

public:
    Exam(const QString& title, const QDate& dueDate, int minuteDuration);

    /**
     * @brief Polymorphic override that formats the child class's
     * unique duration values for the UI table
     */
    QString getDetails() const override;

};

#endif //ASSIGNMENTTRACKER_EXAM_H
