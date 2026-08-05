//
// Created by Chris Silva on 8/5/26.
//

#include "Assignment.h"

Assignment::Assignment(const QString &title,
                       const QDate &dueDate,
                       bool isCompleted,
                       const QString& assignmentTopicInput) {
        m_title = title;
        m_dueDate = dueDate;
        m_isCompleted = isCompleted;
        m_assignmentTopicInput = assignmentTopicInput;
    }

Assignment::~Assignment() = default;

QString Assignment::getTitle() const {
    return m_title;
}

QDate Assignment::getDueDate() const {
    return m_dueDate;
}

bool Assignment::isCompleted() const {
    return m_isCompleted;
}

void Assignment::setCompleted(bool status) {
    m_isCompleted = status;
}
