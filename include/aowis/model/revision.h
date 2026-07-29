#ifndef AOWIS_MODEL_REVISION_H
#define AOWIS_MODEL_REVISION_H

#include <optional>

#include <QDateTime>
#include <QString>
#include <QUuid>

enum class RevisionState
{
    Working,
    Committed
};

struct Revision
{
    QUuid id;
    QUuid project_id;
    std::optional<QUuid> parent_revision_id;
    std::optional<qint64> number;

    RevisionState state = RevisionState::Working;

    QString title;
    QString commit_message;

    QDateTime created_at;
    QDateTime modified_at;
    std::optional<QDateTime> committed_at;
};

#endif // AOWIS_MODEL_REVISION_H
