//
// Created by Chris Silva on 8/5/26.
//

#include "Homework.h"

Homework::Homework(const QString &title, const QDate &dueDate, const QString &topic)
    : Assignment(title, dueDate), m_topic(topic) {
}

QString Homework::getDetails() const {
    return QString("Homework: %1").arg(m_topic);
}
