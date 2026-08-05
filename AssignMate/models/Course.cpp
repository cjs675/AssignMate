//
// Created by Chris Silva on 8/5/26.
//

#include "Course.h"

Course::Course(QString name)
    : m_name(name) {
}

/**
 *  @brief Destructor to clean up memory when the course object
 *         is deleted.
 *         Prevents memory leaks by manually deleting all child
 *         Assignment pointers
 */
Course::~Course() {
    for (Assignment *a: m_assignments) {
        delete a;
    }
}

// --- Course Operations ---
QString Course::getName() const {
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
QVector<Assignment *> &Course::getAssignments() {
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
void Course::addAssignment(Assignment *a) {
    m_assignments.push_back(a);
}

void Course::dropAssignment(int index) {
    if (index >= 0 && index < m_assignments.size()) {
        // remove allocated memory
        delete m_assignments[index];
        // remove pointer from vector of assignments
        m_assignments.removeAt(index);
    }
}
