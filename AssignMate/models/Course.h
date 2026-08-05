//
// Created by Chris Silva on 8/5/26.
//

#ifndef ASSIGNMENTTRACKER_COURSE_H
#define ASSIGNMENTTRACKER_COURSE_H

#include <QString>
#include <QVector>
#include "Assignment.h"

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
    QVector<Assignment *> m_assignments;

public:
    explicit Course(QString name);

    ~Course();

    QString getName() const;

    QVector<Assignment *> &getAssignments();

    void addAssignment(Assignment *assignment);

    void dropAssignment(int index);
};


#endif //ASSIGNMENTTRACKER_COURSE_H
