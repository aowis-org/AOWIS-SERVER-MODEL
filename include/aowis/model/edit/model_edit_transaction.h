#ifndef AOWIS_MODEL_EDIT_TRANSACTION_H
#define AOWIS_MODEL_EDIT_TRANSACTION_H

#include <QByteArray>
#include <QString>
#include <QUuid>
#include <QVector>
#include <optional>

// A logical user operation is one atomic, reversible history step.  The before
// and after images use the canonical entity codec owned by AOWIS-SERVER-MODEL;
// this envelope does not reinterpret model fields or depend on DVMDB.
namespace aowis::model {

struct ModelEntityEdit final
{
    QUuid entityId;
    QString entityType;
    std::optional<QByteArray> before;
    std::optional<QByteArray> after;
};

struct ModelEditTransaction final
{
    QUuid transactionId; // UUIDv7; also the stable retry/idempotency key.
    QString operation;   // Stable logical operation name, e.g. hydraulic.pipe.split.
    QString description; // Human-readable, optional; never used for replay.
    QVector<ModelEntityEdit> edits;
};

// This is an uncompressed, framed, deterministic binary representation.
// Encoded transactions are intentionally small; transport/storage may compress
// *batches* when beneficial.  The 32-byte SHA-256 trailer detects corruption.
// An empty encode result means invalid input; a null decode means invalid data.
class ModelEditTransactionCodec final
{
public:
    static QByteArray encode(const ModelEditTransaction &transaction, QString *error = nullptr);
    static std::optional<ModelEditTransaction> decode(const QByteArray &bytes, QString *error = nullptr);
};

} // namespace aowis::model

#endif // AOWIS_MODEL_EDIT_TRANSACTION_H
