#ifndef AOWIS_MODEL_PROJECT_H
#define AOWIS_MODEL_PROJECT_H

#include <optional>

#include <QDateTime>
#include <QString>
#include <QUuid>

enum class ProjectState
{
    Active,
    Archived
};

struct Project
{
    QString id;
    QUuid uuid;
    QString name;
    QString description;
    QDateTime created_at;
    QDateTime modified_at;
    std::optional<QDateTime> archived_at;

    bool isValid() const
    {
        return !this->uuid.isNull();
    }

    ProjectState state() const
    {
        return this->archived_at.has_value()
            ? ProjectState::Archived
            : ProjectState::Active;
    }
};

#endif // AOWIS_MODEL_PROJECT_H
