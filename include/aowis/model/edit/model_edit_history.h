#ifndef AOWIS_MODEL_EDIT_HISTORY_H
#define AOWIS_MODEL_EDIT_HISTORY_H

#include <aowis/model/edit/model_edit_transaction.h>

#include <QHash>
#include <QByteArray>
#include <QString>
#include <QUuid>

#include <optional>

namespace aowis::model {

// An in-memory canonical-image view, not the native DVMDB entity index.
// A model-specific codec/referential validator must still validate images
// before a history transition is committed by the GUI or DVMDB.
struct ModelHistoryImage final
{
    QString entityType;
    QByteArray bytes;

    bool operator==(const ModelHistoryImage &other) const = default;
};

using ModelHistoryImageMap = QHash<QUuid, ModelHistoryImage>;

// HIST-01: construct new reversible events without mutating an original
// transaction or reusing its durable retry ID. The original action identity,
// history-cursor transition and Commit checkpoint linkage are intentionally
// owned by the future DVMDB history journal (HIST-02).
class ModelEditHistory final
{
public:
    // Swap all before/after images; creations become deletions and vice versa.
    // A fresh UUIDv7 is mandatory, even when the original targets UUIDv4/v5
    // entities. The operation is "history.undo" for a generated inverse.
    static std::optional<ModelEditTransaction> inverse(
        const ModelEditTransaction &original, const QUuid &transactionId,
        QString *error = nullptr);

    // Reapply the same before/after images as a new "history.redo" event.
    static std::optional<ModelEditTransaction> redo(
        const ModelEditTransaction &original, const QUuid &transactionId,
        QString *error = nullptr);

    // Strict before-image compare-and-swap for *all* entities in the event.
    // The caller's state is unchanged on any failure (including a conflict
    // on the last edit); successful deletions remove the typed image entry.
    // This image-level helper does not validate field codecs or dependencies.
    static bool apply(const ModelEditTransaction &transaction,
                      ModelHistoryImageMap &state, QString *error = nullptr);
};

} // namespace aowis::model

#endif // AOWIS_MODEL_EDIT_HISTORY_H
