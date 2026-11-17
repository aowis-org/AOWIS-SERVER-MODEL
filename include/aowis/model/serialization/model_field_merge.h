#ifndef AOWIS_MODEL_SERIALIZATION_MODEL_FIELD_MERGE_H
#define AOWIS_MODEL_SERIALIZATION_MODEL_FIELD_MERGE_H

#include <optional>
#include <QByteArray>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QUuid>

namespace aowis::model
{
// Schema-aware field merge for supported hydraulic distribution entities.
// EntityMetadata is merged by its seven subfields; other nested values and
// collections are atomic fields. Network-root metadata is not supported by
// mergeModelFields and retains entity-level conflict handling.
// field_choices maps an explicitly conflicting field name to whether the
// right side wins. Unknown names are rejected by the commit caller.
struct ModelFieldMerge
{
    QByteArray content; // Nonempty only when conflicting_fields is empty.
    QStringList conflicting_fields;
};

std::optional<ModelFieldMerge> mergeModelFields(const QUuid &object_uuid,
    const QString &kind, const QByteArray &base, const QByteArray &left,
    const QByteArray &right, const QMap<QString, bool> &field_choices = {});

} // namespace aowis::model
#endif // AOWIS_MODEL_SERIALIZATION_MODEL_FIELD_MERGE_H
