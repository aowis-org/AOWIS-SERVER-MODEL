#ifndef AOWIS_MODEL_REVISION_H
#define AOWIS_MODEL_REVISION_H

#include <optional>

#include <QDateTime>
#include <QString>
#include <QUuid>
#include <QtGlobal>

enum class RevisionState
{
    Working,
    Committed
};

struct Revision
{
    QString id;
    QUuid uuid;
    QUuid project_uuid;
    std::optional<QUuid> parent_revision_uuid;
    std::optional<qint64> number;

    RevisionState state = RevisionState::Working;

    QString title;
    QString commit_message;

    QDateTime created_at;
    QDateTime modified_at;
    std::optional<QDateTime> committed_at;
};

#endif // AOWIS_MODEL_REVISION_H
