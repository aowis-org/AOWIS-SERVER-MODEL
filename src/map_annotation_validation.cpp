#include <aowis/model/serialization/model_validation.h>

#include <cmath>

namespace aowis::model
{
namespace
{
void issue(ModelValidationReport &report, const QString &path, const QString &description)
{
    report.issues.append({path, description});
}

bool finitePosition(const CoordinateWGS84 &value)
{
    return std::isfinite(value.longitude_deg) && value.longitude_deg >= -180.0 &&
           value.longitude_deg <= 180.0 && std::isfinite(value.latitude_deg) &&
           value.latitude_deg >= -90.0 && value.latitude_deg <= 90.0 &&
           (!value.altitude_m || std::isfinite(*value.altitude_m));
}

bool supportedAnchorType(const QString &name)
{
    // Anchor support is intentionally narrower than the MODEL entity registry.
    // Line geometry has no unambiguous point-position anchor.
    return name == QStringLiteral("HydraulicNodeJunction") ||
           name == QStringLiteral("HydraulicNodeReservoir") ||
           name == QStringLiteral("HydraulicNodeTank") ||
           name == QStringLiteral("HydraulicDemandPoint");
}
}

ModelValidationReport validateMapAnnotation(const MapAnnotation &value)
{
    ModelValidationReport report;
    if (value.uuid.isNull())
        issue(report, QStringLiteral("annotation.uuid"), QStringLiteral("must not be null"));
    if (value.text.trimmed().isEmpty())
        issue(report, QStringLiteral("annotation.text"), QStringLiteral("must not be blank"));
    if (value.text.size() > 4096)
        issue(report, QStringLiteral("annotation.text"), QStringLiteral("must not exceed 4096 UTF-16 code units"));
    if (!finitePosition(value.position_wgs84))
        issue(report, QStringLiteral("annotation.position_wgs84"), QStringLiteral("invalid WGS84 position"));
    if (value.anchor)
    {
        if (value.anchor->entity_uuid.isNull())
            issue(report, QStringLiteral("annotation.anchor.entity_uuid"), QStringLiteral("must not be null"));
        if (!supportedAnchorType(value.anchor->entity_type))
            issue(report, QStringLiteral("annotation.anchor.entity_type"), QStringLiteral("unsupported point-position entity type"));
        if (!std::isfinite(value.anchor->offset_east_m) ||
            !std::isfinite(value.anchor->offset_north_m))
            issue(report, QStringLiteral("annotation.anchor.offset"), QStringLiteral("offsets must be finite"));
    }
    switch (value.style.text_size)
    {
    case MapAnnotationTextSize::Small:
    case MapAnnotationTextSize::Normal:
    case MapAnnotationTextSize::Large:
        break;
    default:
        issue(report, QStringLiteral("annotation.style.text_size"), QStringLiteral("unknown text size"));
        break;
    }
    return report;
}

ModelValidationReport validateMapAnnotationLayer(const MapAnnotationLayer &value)
{
    ModelValidationReport report;
    if (value.uuid.isNull())
        issue(report, QStringLiteral("annotation_layer.uuid"), QStringLiteral("must not be null"));
    if (value.owner_network_uuid && value.owner_network_uuid->isNull())
        issue(report, QStringLiteral("annotation_layer.owner_network_uuid"), QStringLiteral("must not contain a null UUID"));
    switch (value.domain)
    {
    case MapAnnotationDomain::Distribution:
        if (!value.owner_network_uuid || value.owner_network_uuid->isNull())
            issue(report, QStringLiteral("annotation_layer.owner_network_uuid"), QStringLiteral("distribution layer must own a network"));
        break;
    case MapAnnotationDomain::Drainage:
        if (value.owner_network_uuid)
            issue(report, QStringLiteral("annotation_layer.owner_network_uuid"), QStringLiteral("pre-network drainage layer cannot own a network"));
        break;
    default:
        issue(report, QStringLiteral("annotation_layer.domain"), QStringLiteral("unknown annotation domain"));
        break;
    }
    return report;
}
} // namespace aowis::model
