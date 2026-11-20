#include <aowis/model/map/map_annotation.h>
#include <aowis/model/serialization/model_codec.h>
#include <aowis/model/serialization/model_validation.h>

#include <QByteArray>
#include <QChar>
#include <QDataStream>
#include <QIODevice>
#include <QList>
#include <QUuid>
#include <QString>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <optional>

namespace
{
MapAnnotation validAnnotation()
{
    MapAnnotation result;
    result.uuid = QUuid::createUuid();
    result.text = QStringLiteral("Pump station – planned refurbishment");
    result.position_wgs84.longitude_deg = 13.405;
    result.position_wgs84.latitude_deg = 52.52;
    result.position_wgs84.altitude_m = 42.0;
    result.style.text_size = MapAnnotationTextSize::Large;
    result.style.show_background = true;
    return result;
}

bool passes(const MapAnnotation &annotation)
{
    return aowis::model::validateMapAnnotation(annotation).valid();
}

bool refuses(const MapAnnotation &annotation)
{
    return !passes(annotation) &&
           aowis::model::ModelCodec<MapAnnotation>::encode(annotation).isEmpty();
}
}

int main()
{
    using aowis::model::ModelCodec;
    MapAnnotation annotation = validAnnotation();
    if (!passes(annotation))
        return EXIT_FAILURE;
    const QByteArray encoded = ModelCodec<MapAnnotation>::encode(annotation);
    const std::optional<MapAnnotation> decoded = ModelCodec<MapAnnotation>::decode(encoded);
    if (encoded.isEmpty() || !decoded || decoded->uuid != annotation.uuid ||
        decoded->text != annotation.text || decoded->position_wgs84.altitude_m != 42.0 ||
        decoded->style.text_size != MapAnnotationTextSize::Large ||
        !decoded->style.show_background || !decoded->style.visible ||
        ModelCodec<MapAnnotation>::encode(*decoded) != encoded)
        return EXIT_FAILURE;

    QString error;
    if (ModelCodec<MapAnnotation>::decode(encoded.left(encoded.size() - 1), &error) || error.isEmpty())
        return EXIT_FAILURE;
    if (ModelCodec<MapAnnotationLayer>::decode(encoded))
        return EXIT_FAILURE; // No cross-type payloads.

    annotation.text = QStringLiteral(" \t ");
    if (!refuses(annotation)) return EXIT_FAILURE;
    annotation = validAnnotation();
    annotation.text = QString(4097, QLatin1Char('x'));
    if (!refuses(annotation)) return EXIT_FAILURE;

    // A8: plain text only. Each case is refused on its own.
    const QString lead = QStringLiteral("Note ");
    const QList<QString> refusedTexts{
        lead + QChar(0xD800),                              // unpaired high surrogate
        lead + QChar(0xDC00) + QStringLiteral("x"),        // unpaired low surrogate
        lead + QChar(0x0000),                              // NUL
        lead + QChar(0x001B) + QStringLiteral("[31m"),     // ESC sequence
        lead + QChar(0x000D),                              // carriage return
        lead + QChar(0x007F),                              // DEL
        lead + QChar(0x0085),                              // C1 next line
        lead + QChar(0x202E) + QStringLiteral("txt.exe"),  // right-to-left override
        lead + QChar(0x2066) + QStringLiteral("x"),        // left-to-right isolate
        QString(QChar(0x200B)),                            // only a zero-width space
        QString(QChar(0x200B)) + QStringLiteral(" \n\t") + QChar(0x2060), // only invisible
        QString(QChar(0x3000)),                            // only ideographic space
    };
    for (const QString &text : refusedTexts)
    {
        annotation = validAnnotation();
        annotation.text = text;
        if (!refuses(annotation)) return EXIT_FAILURE;
    }
    const QList<QString> acceptedTexts{
        QStringLiteral("Line one\nLine two\twith tab"),
        QString(QChar(0x200B)) + QStringLiteral("a"),
        QString::fromUtf8("Valve \xF0\x9F\x94\xA7 OK"),          // surrogate pair (emoji)
        QString::fromUtf8("\xD7\xA9\xD7\x9C\xD7\x95\xD7\x9D") + QChar(0x200F), // Hebrew + RLM mark
        QString(4096, QLatin1Char('y')),
    };
    for (const QString &text : acceptedTexts)
    {
        annotation = validAnnotation();
        annotation.text = text;
        if (!passes(annotation)) return EXIT_FAILURE;
        const QByteArray bytes = ModelCodec<MapAnnotation>::encode(annotation);
        const std::optional<MapAnnotation> back = ModelCodec<MapAnnotation>::decode(bytes);
        if (bytes.isEmpty() || !back || back->text != text) return EXIT_FAILURE;
    }
    annotation = validAnnotation();
    annotation.uuid = QUuid{};
    if (!refuses(annotation)) return EXIT_FAILURE;
    annotation = validAnnotation();
    annotation.position_wgs84.longitude_deg = 181.0;
    if (!refuses(annotation)) return EXIT_FAILURE;
    annotation = validAnnotation();
    annotation.position_wgs84.latitude_deg = -91.0;
    if (!refuses(annotation)) return EXIT_FAILURE;
    annotation = validAnnotation();
    annotation.position_wgs84.altitude_m = std::numeric_limits<double>::quiet_NaN();
    if (!refuses(annotation)) return EXIT_FAILURE;
    annotation = validAnnotation();
    annotation.style.text_size = static_cast<MapAnnotationTextSize>(900);
    if (!refuses(annotation)) return EXIT_FAILURE;

    annotation = validAnnotation();
    MapAnnotationAnchor anchor;
    anchor.entity_uuid = QUuid::createUuid();
    anchor.entity_type = QStringLiteral("HydraulicNodeTank");
    anchor.offset_east_m = -23.4;
    anchor.offset_north_m = 5.6;
    annotation.anchor = anchor;
    if (!passes(annotation)) return EXIT_FAILURE;
    const QByteArray anchored = ModelCodec<MapAnnotation>::encode(annotation);
    const std::optional<MapAnnotation> recovered = ModelCodec<MapAnnotation>::decode(anchored);
    if (anchored.isEmpty() || !recovered || !recovered->anchor ||
        recovered->anchor->entity_uuid != anchor.entity_uuid ||
        recovered->anchor->offset_east_m != -23.4 ||
        recovered->anchor->offset_north_m != 5.6)
        return EXIT_FAILURE;
    annotation.anchor->entity_type = QStringLiteral("HydraulicLinkPipe");
    if (!refuses(annotation)) return EXIT_FAILURE;
    annotation.anchor->entity_type = QStringLiteral("HydraulicNodeTank");
    annotation.anchor->entity_uuid = QUuid{};
    if (!refuses(annotation)) return EXIT_FAILURE;
    annotation.anchor->entity_uuid = QUuid::createUuid();
    annotation.anchor->offset_east_m = std::numeric_limits<double>::infinity();
    if (!refuses(annotation)) return EXIT_FAILURE;

    MapAnnotationLayer layer;
    layer.uuid = QUuid::createUuid();
    layer.domain = MapAnnotationDomain::Distribution;
    layer.owner_network_uuid = QUuid::createUuid();
    if (!aowis::model::validateMapAnnotationLayer(layer).valid())
        return EXIT_FAILURE;
    const QByteArray layerBytes = ModelCodec<MapAnnotationLayer>::encode(layer);
    const std::optional<MapAnnotationLayer> decodedLayer = ModelCodec<MapAnnotationLayer>::decode(layerBytes);
    if (layerBytes.isEmpty() || !decodedLayer || decodedLayer->uuid != layer.uuid ||
        decodedLayer->owner_network_uuid != layer.owner_network_uuid ||
        ModelCodec<MapAnnotationLayer>::encode(*decodedLayer) != layerBytes)
        return EXIT_FAILURE;
    layer.owner_network_uuid.reset();
    if (aowis::model::validateMapAnnotationLayer(layer).valid() ||
        !ModelCodec<MapAnnotationLayer>::encode(layer).isEmpty())
        return EXIT_FAILURE;
    layer.domain = MapAnnotationDomain::Drainage;
    if (!aowis::model::validateMapAnnotationLayer(layer).valid() ||
        ModelCodec<MapAnnotationLayer>::encode(layer).isEmpty())
        return EXIT_FAILURE;
    layer.owner_network_uuid = QUuid::createUuid();
    if (aowis::model::validateMapAnnotationLayer(layer).valid())
        return EXIT_FAILURE;
    layer.owner_network_uuid.reset();
    layer.domain = static_cast<MapAnnotationDomain>(42);
    if (aowis::model::validateMapAnnotationLayer(layer).valid() ||
        !ModelCodec<MapAnnotationLayer>::encode(layer).isEmpty())
        return EXIT_FAILURE;

    // Both nested public codecs enforce the SAME semantic constraints as an
    // embedded anchor/style. An invalid value cannot escape via a standalone
    // codec, and valid bytes still have deterministic round trips.
    anchor = MapAnnotationAnchor{};
    anchor.entity_uuid = QUuid::createUuid();
    anchor.entity_type = QStringLiteral("HydraulicNodeTank");
    anchor.offset_east_m = 12.5;
    anchor.offset_north_m = -8.0;
    const QByteArray anchorBytes = ModelCodec<MapAnnotationAnchor>::encode(anchor);
    if (anchorBytes.isEmpty() ||
        !ModelCodec<MapAnnotationAnchor>::decode(anchorBytes) ||
        ModelCodec<MapAnnotationAnchor>::encode(*ModelCodec<MapAnnotationAnchor>::decode(anchorBytes)) != anchorBytes)
        return EXIT_FAILURE;
    if (aowis::model::mapAnnotationAnchorTargetDomain(anchor.entity_type) !=
            MapAnnotationDomain::Distribution ||
        aowis::model::mapAnnotationAnchorTargetDomain(QStringLiteral("HydraulicNodeJunction")) !=
            MapAnnotationDomain::Distribution ||
        aowis::model::mapAnnotationAnchorTargetDomain(QStringLiteral("HydraulicNodeReservoir")) !=
            MapAnnotationDomain::Distribution ||
        aowis::model::mapAnnotationAnchorTargetDomain(QStringLiteral("HydraulicDemandPoint")) !=
            MapAnnotationDomain::Distribution ||
        aowis::model::mapAnnotationAnchorTargetDomain(QStringLiteral("HydraulicLinkPipe")) ||
        aowis::model::mapAnnotationAnchorTargetDomain(QStringLiteral("DrainageNode")))
        return EXIT_FAILURE;
    anchor.entity_uuid = QUuid{};
    if (aowis::model::validateMapAnnotationAnchor(anchor).valid() ||
        !ModelCodec<MapAnnotationAnchor>::encode(anchor).isEmpty())
        return EXIT_FAILURE;
    anchor.entity_uuid = QUuid::createUuid();
    anchor.entity_type = QStringLiteral("HydraulicLinkPipe");
    if (aowis::model::validateMapAnnotationAnchor(anchor).valid() ||
        !ModelCodec<MapAnnotationAnchor>::encode(anchor).isEmpty())
        return EXIT_FAILURE;
    anchor.entity_type = QStringLiteral("HydraulicNodeTank");
    anchor.offset_north_m = std::numeric_limits<double>::infinity();
    if (aowis::model::validateMapAnnotationAnchor(anchor).valid() ||
        !ModelCodec<MapAnnotationAnchor>::encode(anchor).isEmpty())
        return EXIT_FAILURE;
    anchor.offset_north_m = 0.0;
    anchor.offset_east_m = std::numeric_limits<double>::quiet_NaN();
    if (aowis::model::validateMapAnnotationAnchor(anchor).valid() ||
        !ModelCodec<MapAnnotationAnchor>::encode(anchor).isEmpty())
        return EXIT_FAILURE;

    MapAnnotationStyle style = validAnnotation().style;
    const QByteArray styleBytes = ModelCodec<MapAnnotationStyle>::encode(style);
    if (styleBytes.isEmpty() || !ModelCodec<MapAnnotationStyle>::decode(styleBytes) ||
        ModelCodec<MapAnnotationStyle>::encode(*ModelCodec<MapAnnotationStyle>::decode(styleBytes)) != styleBytes)
        return EXIT_FAILURE;
    style.text_size = static_cast<MapAnnotationTextSize>(900);
    if (aowis::model::validateMapAnnotationStyle(style).valid() ||
        !ModelCodec<MapAnnotationStyle>::encode(style).isEmpty())
        return EXIT_FAILURE;

    // A syntactically valid, correctly typed envelope with an unsupported
    // string key must be refused by semantic decode (not just by encode).
    // Both strings have the same UTF-16 encoded length.
    QByteArray validType;
    QByteArray invalidType;
    {
        QDataStream writer(&validType, QIODevice::WriteOnly);
        writer.setVersion(QDataStream::Qt_6_0);
        writer << QStringLiteral("HydraulicNodeTank");
    }
    {
        QDataStream writer(&invalidType, QIODevice::WriteOnly);
        writer.setVersion(QDataStream::Qt_6_0);
        writer << QStringLiteral("HydraulicLinkPipe");
    }
    QByteArray invalidAnchorBytes = anchorBytes;
    if (validType.size() != invalidType.size() ||
        !invalidAnchorBytes.contains(validType))
        return EXIT_FAILURE;
    invalidAnchorBytes.replace(validType, invalidType);
    QString validationError;
    if (ModelCodec<MapAnnotationAnchor>::decode(invalidAnchorBytes, &validationError) ||
        !validationError.contains(QStringLiteral("unsupported")))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
