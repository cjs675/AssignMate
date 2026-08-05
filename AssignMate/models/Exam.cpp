//
// Created by Chris Silva on 8/5/26.
//

#include "Exam.h"

Exam::Exam(const QString& title, const QDate& dueDate, int minuteDuration)
    : Assignment(title, dueDate),
      m_minuteDuration(minuteDuration) {}

QString Exam::getDetails() const {
    return QString("Exam: %1 Minutes").arg(m_minuteDuration);
}
