#include <aowis/model/edit/model_edit_transaction.h>

#include <QCryptographicHash>
#include <QByteArray>
#include <QString>
#include <QUuid>

#include <cstdlib>
#include <iostream>
#include <optional>
#include <utility>

using aowis::model::ModelEditTransaction;
using aowis::model::ModelEditTransactionCodec;
using aowis::model::ModelEntityEdit;

namespace {
void require(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << "Model edit transaction test failed: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

QUuid id(const char *suffix)
{
    return QUuid(QStringLiteral("019a0234-5678-7a12-8a01-") + QString::fromLatin1(suffix));
}

void seal(QByteArray &frame)
{
    const QByteArray payload = frame.mid(9, frame.size() - 9 - 32);
    frame.replace(frame.size() - 32, 32, QCryptographicHash::hash(payload, QCryptographicHash::Sha256));
}
} // namespace

int main()
{
    ModelEditTransaction transaction;
    transaction.transactionId = id("000000000003");
    transaction.operation = QStringLiteral("hydraulic.pipe.split");
    transaction.description = QString::fromUtf8("Split pipe – Prüfstelle");
    transaction.edits = {
        ModelEntityEdit{id("000000000005"), QStringLiteral("HydraulicLinkPipe"), QByteArray("old\0state", 9), QByteArray("new\0state", 9)},
        ModelEntityEdit{id("000000000006"), QStringLiteral("HydraulicNodeJunction"), std::nullopt, QByteArray("node", 4)},
        ModelEntityEdit{id("000000000007"), QStringLiteral("HydraulicLinkPipe"), QByteArray("pipe", 4), std::nullopt}
    };
    QString error;
    const QByteArray frame = ModelEditTransactionCodec::encode(transaction, &error);
    require(!frame.isEmpty() && error.isEmpty(), "encode valid logical operation");
    require(frame.size() < 512, "small operations remain small without ZIP or JSON");
    require(frame == ModelEditTransactionCodec::encode(transaction), "deterministic bytes");
    const std::optional<ModelEditTransaction> decoded = ModelEditTransactionCodec::decode(frame, &error);
    require(decoded.has_value() && error.isEmpty(), "decode valid operation");
    require(decoded->transactionId == transaction.transactionId &&
            decoded->operation == transaction.operation && decoded->description == transaction.description &&
            decoded->edits.size() == transaction.edits.size(), "round-trip metadata");
    for (qsizetype i = 0; i < transaction.edits.size(); ++i) {
        require(decoded->edits.at(i).entityId == transaction.edits.at(i).entityId &&
                decoded->edits.at(i).entityType == transaction.edits.at(i).entityType &&
                decoded->edits.at(i).before == transaction.edits.at(i).before &&
                decoded->edits.at(i).after == transaction.edits.at(i).after,
                "byte-exact reversible edit round-trip");
    }
    QByteArray corrupted = frame;
    corrupted[50] = static_cast<char>(corrupted.at(50) ^ 0x01);
    require(!ModelEditTransactionCodec::decode(corrupted, &error).has_value() &&
            error.contains(QStringLiteral("digest")), "corruption detected by digest");
    require(!ModelEditTransactionCodec::decode(frame.left(frame.size() - 1)).has_value(), "truncated digest rejected");
    require(!ModelEditTransactionCodec::decode(frame + 'x').has_value(), "trailing bytes rejected");
    corrupted = frame;
    corrupted[4] = static_cast<char>(2);
    require(!ModelEditTransactionCodec::decode(corrupted).has_value(), "unknown format rejected");
    corrupted = frame;
    corrupted[35] = static_cast<char>(0xff); // operation UTF-8, then rehash: semantic validation remains mandatory.
    seal(corrupted);
    require(!ModelEditTransactionCodec::decode(corrupted).has_value(), "invalid UTF-8 rejected even with valid digest");
    transaction.edits[1].entityId = transaction.edits[0].entityId;
    require(ModelEditTransactionCodec::encode(transaction).isEmpty(), "duplicate entities rejected");
    transaction.edits[1].entityId = id("000000000006");
    std::swap(transaction.edits[0], transaction.edits[1]);
    require(ModelEditTransactionCodec::encode(transaction) == frame, "insertion order does not affect canonical bytes");
    std::swap(transaction.edits[0], transaction.edits[1]);
    transaction.edits[0].after = transaction.edits[0].before;
    require(ModelEditTransactionCodec::encode(transaction).isEmpty(), "no-op edits rejected");
    transaction.edits[0].after = QByteArray("new", 3);
    transaction.transactionId = QUuid(QStringLiteral("019a0234-5678-4a12-8a01-000000000003"));
    require(ModelEditTransactionCodec::encode(transaction).isEmpty(), "non-v7 transaction ids rejected");
    transaction.transactionId = id("000000000003");
    transaction.edits[0].after = QByteArray(1024 * 1024 + 1, 'x');
    require(ModelEditTransactionCodec::encode(transaction).isEmpty(), "oversized entity payload rejected");
    transaction.edits[0].after = QByteArray("new", 3);

    // Imported EPANET entities carry deterministic UUIDv5 model keys and
    // older MODEL data may carry UUIDv4: editing them must be representable.
    ModelEditTransaction imported;
    imported.transactionId = id("000000000010");
    imported.operation = QStringLiteral("hydraulic.pipe.diameter");
    imported.edits = {
        ModelEntityEdit{QUuid(QStringLiteral("6f1c2d3e-4b5a-5c6d-9e8f-0a1b2c3d4e5f")),
                        QStringLiteral("HydraulicLinkPipe"), QByteArray("d100", 4), QByteArray("d150", 4)},
        ModelEntityEdit{QUuid(QStringLiteral("1b2c3d4e-5f60-4172-8a93-b4c5d6e7f809")),
                        QStringLiteral("HydraulicNodeJunction"), QByteArray("n", 1), std::nullopt}
    };
    const QByteArray imported_frame = ModelEditTransactionCodec::encode(imported, &error);
    require(!imported_frame.isEmpty() && error.isEmpty(), "UUIDv5/UUIDv4 entity keys encode");
    const std::optional<ModelEditTransaction> imported_decoded =
        ModelEditTransactionCodec::decode(imported_frame, &error);
    require(imported_decoded.has_value() && error.isEmpty() &&
            imported_decoded->edits.size() == 2 &&
            ModelEditTransactionCodec::encode(*imported_decoded) == imported_frame,
            "UUIDv5/UUIDv4 entity keys round-trip byte-exactly");
    imported.edits[1].entityId = QUuid();
    require(ModelEditTransactionCodec::encode(imported).isEmpty(), "null entity UUID rejected");
    imported.edits[1].entityId = QUuid(QStringLiteral("1b2c3d4e-5f60-4172-8a93-b4c5d6e7f809"));
    imported.transactionId = QUuid(QStringLiteral("6f1c2d3e-4b5a-5c6d-9e8f-0a1b2c3d4e5f"));
    require(ModelEditTransactionCodec::encode(imported).isEmpty(),
            "transaction UUID must still be UUIDv7");

    // A frame with a null entity UUID is rejected by decode, too.
    QByteArray null_entity = frame;
    // payload: 16 transaction + (2 + operation) + (2 + description) + 2 count
    const qsizetype first_entity = 9 + 16 + 2 + QByteArray("hydraulic.pipe.split").size() + 2 +
        QString::fromUtf8("Split pipe – Prüfstelle").toUtf8().size() + 2;
    null_entity.replace(first_entity, 16, QByteArray(16, '\0'));
    seal(null_entity);
    require(!ModelEditTransactionCodec::decode(null_entity).has_value(), "null entity UUID rejected on decode");

    std::cout << "Model edit transaction codec checks passed\n";
    return EXIT_SUCCESS;
}
