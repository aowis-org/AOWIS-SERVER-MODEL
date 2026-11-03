#include <aowis/model/edit/model_edit_history.h>

#include <utility>

namespace aowis::model {
namespace {
void fail(QString *error, const QString &message)
{
    if (error) *error = message;
}

bool validate(const ModelEditTransaction &transaction, QString *error)
{
    const QByteArray bytes = ModelEditTransactionCodec::encode(transaction, error);
    if (!bytes.isEmpty()) return true;
    if (error && error->isEmpty())
        *error = QStringLiteral("Invalid MODEL history transaction");
    return false;
}

std::optional<ModelEditTransaction> newHistoryEvent(
    const ModelEditTransaction &original, const QUuid &transactionId,
    bool inverse, QString *error)
{
    if (error) error->clear();
    if (!validate(original, error)) return std::nullopt;
    if (transactionId == original.transactionId) {
        fail(error, QStringLiteral("A history operation requires a fresh transaction UUID"));
        return std::nullopt;
    }

    ModelEditTransaction generated;
    generated.transactionId = transactionId;
    generated.operation = inverse ? QStringLiteral("history.undo") : QStringLiteral("history.redo");
    generated.description = original.description;
    generated.edits = original.edits;
    if (inverse) {
        for (ModelEntityEdit &edit : generated.edits)
            std::swap(edit.before, edit.after);
    }
    // This checks the generated event ID and all inherited codec constraints,
    // while preserving the identical canonical before/after image bytes.
    if (!validate(generated, error)) return std::nullopt;
    return generated;
}
} // namespace

std::optional<ModelEditTransaction> ModelEditHistory::inverse(
    const ModelEditTransaction &original, const QUuid &transactionId, QString *error)
{
    return newHistoryEvent(original, transactionId, true, error);
}

std::optional<ModelEditTransaction> ModelEditHistory::redo(
    const ModelEditTransaction &original, const QUuid &transactionId, QString *error)
{
    return newHistoryEvent(original, transactionId, false, error);
}

bool ModelEditHistory::apply(const ModelEditTransaction &transaction,
                             ModelHistoryImageMap &state, QString *error)
{
    if (error) error->clear();
    if (!validate(transaction, error)) return false;

    // Preflight *every* before-image against one consistent starting state.
    // Do not change the caller's map until the entire transaction has passed.
    for (const ModelEntityEdit &edit : transaction.edits) {
        const ModelHistoryImageMap::const_iterator found = state.constFind(edit.entityId);
        if (!edit.before) {
            if (found != state.cend()) {
                fail(error, QStringLiteral("History creation conflicts with an existing entity"));
                return false;
            }
        } else {
            if (found == state.cend()) {
                fail(error, QStringLiteral("History before-image target does not exist"));
                return false;
            }
            if (found->entityType != edit.entityType || found->bytes != *edit.before) {
                fail(error, QStringLiteral("History before-image or entity type mismatch"));
                return false;
            }
        }
    }

    ModelHistoryImageMap updated = state;
    for (const ModelEntityEdit &edit : transaction.edits) {
        if (edit.after) {
            ModelHistoryImage replacement;
            replacement.entityType = edit.entityType;
            replacement.bytes = *edit.after;
            updated.insert(edit.entityId, std::move(replacement));
        } else {
            updated.remove(edit.entityId);
        }
    }
    state.swap(updated);
    return true;
}

} // namespace aowis::model
