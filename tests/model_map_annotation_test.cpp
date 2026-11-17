#include <aowis/model/map/map_annotation.h>
#include <aowis/model/serialization/model_codec.h>
#include <aowis/model/serialization/model_validation.h>

#include <QByteArray>
#include <QUuid>
#include <QString>
#include <cmath>
#include <cstdlib>
#include <limits>

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

    // Both nested public codecs have deterministic standalone round trips.
    if (!ModelCodec<MapAnnotationAnchor>::decode(ModelCodec<MapAnnotationAnchor>::encode(anchor)))
        return EXIT_FAILURE;
    if (!ModelCodec<MapAnnotationStyle>::decode(ModelCodec<MapAnnotationStyle>::encode(validAnnotation().style)))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
