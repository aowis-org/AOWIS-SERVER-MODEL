#include <aowis/model/edit/model_edit_transaction.h>

#include <QCryptographicHash>
#include <QStringDecoder>
#include <QtGlobal>

#include <algorithm>
#include <cstdint>
#include <utility>
#include <limits>

namespace aowis::model {
namespace {
constexpr char magic[] = {'A', 'O', 'W', 'E'};
constexpr quint8 version = 1;
constexpr quint32 maxPayloadBytes = 8u * 1024u * 1024u;
constexpr quint32 maxEntityBytes = 1024u * 1024u;
constexpr quint16 maxEdits = 4096;
constexpr quint16 maxOperationBytes = 128;
constexpr quint16 maxDescriptionBytes = 512;
constexpr quint16 maxTypeBytes = 128;
constexpr quint32 absentImage = std::numeric_limits<quint32>::max();
constexpr qsizetype headerBytes = 9;
constexpr qsizetype digestBytes = 32;

void failure(QString *error, const QString &message)
{
    if (error) *error = message;
}

bool uuidV7(const QUuid &uuid)
{
    if (uuid.isNull()) return false;
    const QByteArray value = uuid.toRfc4122();
    return value.size() == 16 &&
        ((static_cast<unsigned char>(value.at(6)) >> 4) == 7) &&
        ((static_cast<unsigned char>(value.at(8)) & 0xc0u) == 0x80u);
}

// Entity identities are model keys, not editor-generated event identities.
// Networks imported from EPANET carry deterministic UUIDv5 entity keys (and
// older MODEL data may carry other versions); DVMDB and the GUI accept any
// non-null canonical UUID as a model key. Requiring UUIDv7 here made every
// edit of such an entity fail at capture time. The transaction UUID, which
// the editor creates for each event and DVMDB uses as its retry key, stays
// UUIDv7.
bool entityUuid(const QUuid &uuid)
{
    return !uuid.isNull();
}

bool asciiIdentifier(const QString &name)
{
    if (name.isEmpty()) return false;
    for (qsizetype i = 0; i < name.size(); ++i) {
        const ushort code = name.at(i).unicode();
        if (i == 0 && !((code >= 'a' && code <= 'z') || (code >= 'A' && code <= 'Z')))
            return false;
        if (!((code >= 'a' && code <= 'z') || (code >= 'A' && code <= 'Z') ||
              (code >= '0' && code <= '9') || code == '.' || code == '_' || code == '-'))
            return false;
    }
    return true;
}

void put16(QByteArray &out, quint16 value)
{
    out.append(static_cast<char>(value >> 8));
    out.append(static_cast<char>(value));
}

void put32(QByteArray &out, quint32 value)
{
    out.append(static_cast<char>(value >> 24));
    out.append(static_cast<char>(value >> 16));
    out.append(static_cast<char>(value >> 8));
    out.append(static_cast<char>(value));
}

class Reader final
{
public:
    explicit Reader(const QByteArray &data) : data_(data) {}
    bool readBytes(qsizetype count, QByteArray &result)
    {
        if (count < 0 || count > this->data_.size() - this->position_) return false;
        result = this->data_.mid(this->position_, count);
        this->position_ += count;
        return true;
    }
    bool read16(quint16 &value)
    {
        if (this->data_.size() - this->position_ < 2) return false;
        const unsigned char *bytes = reinterpret_cast<const unsigned char *>(this->data_.constData() + this->position_);
        value = static_cast<quint16>((static_cast<quint16>(bytes[0]) << 8) | bytes[1]);
        this->position_ += 2;
        return true;
    }
    bool read32(quint32 &value)
    {
        if (this->data_.size() - this->position_ < 4) return false;
        const unsigned char *bytes = reinterpret_cast<const unsigned char *>(this->data_.constData() + this->position_);
        value = (static_cast<quint32>(bytes[0]) << 24) | (static_cast<quint32>(bytes[1]) << 16) |
                (static_cast<quint32>(bytes[2]) << 8) | bytes[3];
        this->position_ += 4;
        return true;
    }
    bool atEnd() const { return this->position_ == this->data_.size(); }
private:
    const QByteArray &data_;
    qsizetype position_ = 0;
};

bool appendImage(QByteArray &data, const std::optional<QByteArray> &image, QString *error)
{
    if (!image) {
        put32(data, absentImage);
        return true;
    }
    if (image->isEmpty() || image->size() > maxEntityBytes) {
        failure(error, QStringLiteral("Entity image must contain 1..1048576 bytes"));
        return false;
    }
    put32(data, static_cast<quint32>(image->size()));
    data.append(*image);
    return true;
}

bool readImage(Reader &reader, std::optional<QByteArray> &image)
{
    quint32 length = 0;
    if (!reader.read32(length)) return false;
    if (length == absentImage) {
        image.reset();
        return true;
    }
    if (!length || length > maxEntityBytes) return false;
    QByteArray bytes;
    if (!reader.readBytes(length, bytes)) return false;
    image = std::move(bytes);
    return true;
}

bool readUtf8(Reader &reader, quint16 maximum, QString &value)
{
    quint16 length = 0;
    if (!reader.read16(length) || length > maximum) return false;
    QByteArray bytes;
    if (!reader.readBytes(length, bytes)) return false;
    QStringDecoder decoder(QStringDecoder::Utf8);
    value = decoder.decode(bytes);
    return !decoder.hasError() && value.toUtf8() == bytes;
}

bool appendUtf8(QByteArray &output, const QString &value, quint16 maximum, QString *error)
{
    const QByteArray bytes = value.toUtf8();
    if (bytes.size() > maximum || QString::fromUtf8(bytes) != value) {
        failure(error, QStringLiteral("Invalid or oversized UTF-8 field"));
        return false;
    }
    put16(output, static_cast<quint16>(bytes.size()));
    output.append(bytes);
    return true;
}
} // namespace

QByteArray ModelEditTransactionCodec::encode(const ModelEditTransaction &transaction, QString *error)
{
    if (error) error->clear();
    if (!uuidV7(transaction.transactionId) || !asciiIdentifier(transaction.operation) ||
        transaction.edits.isEmpty() || transaction.edits.size() > maxEdits) {
        failure(error, QStringLiteral("Invalid transaction UUID, operation or number of edits"));
        return {};
    }
    QByteArray payload;
    payload.append(transaction.transactionId.toRfc4122());
    if (!appendUtf8(payload, transaction.operation, maxOperationBytes, error) ||
        !appendUtf8(payload, transaction.description, maxDescriptionBytes, error)) return {};
    // Sorting belongs to the codec: the same logical set of edits must yield
    // exactly the same bytes, regardless of insertion order in the editor.
    QVector<ModelEntityEdit> edits = transaction.edits;
    std::sort(edits.begin(), edits.end(), [](const ModelEntityEdit &left, const ModelEntityEdit &right) {
        return left.entityId.toRfc4122() < right.entityId.toRfc4122();
    });
    put16(payload, static_cast<quint16>(edits.size()));
    QByteArray lastId;
    for (const ModelEntityEdit &edit : edits) {
        const QByteArray id = edit.entityId.toRfc4122();
        if (!entityUuid(edit.entityId) || (!lastId.isEmpty() && id <= lastId) ||
            !asciiIdentifier(edit.entityType) || (!edit.before && !edit.after) ||
            (edit.before && edit.after && *edit.before == *edit.after)) {
            failure(error, QStringLiteral("Entity edits must be valid, changed and have unique non-null UUIDs"));
            return {};
        }
        lastId = id;
        payload.append(id);
        if (!appendUtf8(payload, edit.entityType, maxTypeBytes, error) ||
            !appendImage(payload, edit.before, error) || !appendImage(payload, edit.after, error)) return {};
        if (payload.size() > maxPayloadBytes) {
            failure(error, QStringLiteral("Transaction exceeds 8 MiB"));
            return {};
        }
    }
    QByteArray result;
    result.reserve(headerBytes + payload.size() + digestBytes);
    result.append(magic, 4);
    result.append(static_cast<char>(version));
    put32(result, static_cast<quint32>(payload.size()));
    result.append(payload);
    result.append(QCryptographicHash::hash(payload, QCryptographicHash::Sha256));
    return result;
}

std::optional<ModelEditTransaction> ModelEditTransactionCodec::decode(const QByteArray &bytes, QString *error)
{
    if (error) error->clear();
    if (bytes.size() < headerBytes + digestBytes || bytes.left(4) != QByteArray(magic, 4) ||
        static_cast<quint8>(bytes.at(4)) != version) {
        failure(error, QStringLiteral("Invalid edit transaction frame"));
        return std::nullopt;
    }
    Reader header(bytes);
    QByteArray ignored;
    quint32 payloadLength = 0;
    if (!header.readBytes(5, ignored) || !header.read32(payloadLength) ||
        payloadLength > maxPayloadBytes ||
        bytes.size() != headerBytes + static_cast<qsizetype>(payloadLength) + digestBytes) {
        failure(error, QStringLiteral("Invalid edit transaction length"));
        return std::nullopt;
    }
    const QByteArray payload = bytes.mid(headerBytes, payloadLength);
    if (QCryptographicHash::hash(payload, QCryptographicHash::Sha256) != bytes.right(digestBytes)) {
        failure(error, QStringLiteral("Edit transaction digest mismatch"));
        return std::nullopt;
    }
    Reader reader(payload);
    ModelEditTransaction transaction;
    QByteArray id;
    quint16 editCount = 0;
    if (!reader.readBytes(16, id)) goto invalid;
    transaction.transactionId = QUuid::fromRfc4122(id);
    if (!uuidV7(transaction.transactionId) ||
        !readUtf8(reader, maxOperationBytes, transaction.operation) ||
        !asciiIdentifier(transaction.operation) ||
        !readUtf8(reader, maxDescriptionBytes, transaction.description) ||
        !reader.read16(editCount) || !editCount || editCount > maxEdits) goto invalid;
    {
        QByteArray lastId;
        for (quint16 i = 0; i < editCount; ++i) {
            ModelEntityEdit edit;
            QByteArray currentId;
            if (!reader.readBytes(16, currentId) || (!lastId.isEmpty() && currentId <= lastId)) goto invalid;
            lastId = currentId;
            edit.entityId = QUuid::fromRfc4122(currentId);
            if (!entityUuid(edit.entityId) || !readUtf8(reader, maxTypeBytes, edit.entityType) ||
                !asciiIdentifier(edit.entityType) || !readImage(reader, edit.before) ||
                !readImage(reader, edit.after) || (!edit.before && !edit.after) ||
                (edit.before && edit.after && *edit.before == *edit.after)) goto invalid;
            transaction.edits.append(std::move(edit));
        }
    }
    if (!reader.atEnd()) goto invalid;
    return transaction;
invalid:
    failure(error, QStringLiteral("Malformed edit transaction payload"));
    return std::nullopt;
}
} // namespace aowis::model
