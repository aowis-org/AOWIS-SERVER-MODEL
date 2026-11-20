#include <aowis/model/serialization/model_validation.h>

#include <QChar>
#include <QLatin1StringView>
#include <QList>

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

struct AnchorTargetType
{
    const char *canonical_entity_type;
    MapAnnotationDomain domain;
};

// The single MODEL catalogue of entities with an unambiguous WGS84 point
// position. DVMDB uses the same registry before resolving an attachment.
constexpr AnchorTargetType anchorTargetTypes[] = {
    {"HydraulicNodeJunction", MapAnnotationDomain::Distribution},
    {"HydraulicNodeReservoir", MapAnnotationDomain::Distribution},
    {"HydraulicNodeTank", MapAnnotationDomain::Distribution},
    {"HydraulicDemandPoint", MapAnnotationDomain::Distribution}
};

void validateAnchor(const MapAnnotationAnchor &value, const QString &prefix,
                    ModelValidationReport &report)
{
    if (value.entity_uuid.isNull())
        issue(report, prefix + QStringLiteral(".entity_uuid"), QStringLiteral("must not be null"));
    if (!mapAnnotationAnchorTargetDomain(value.entity_type))
        issue(report, prefix + QStringLiteral(".entity_type"),
              QStringLiteral("unsupported point-position entity type"));
    if (!std::isfinite(value.offset_east_m) || !std::isfinite(value.offset_north_m))
        issue(report, prefix + QStringLiteral(".offset"), QStringLiteral("offsets must be finite"));
}

// Annotation text is plain text (A8): well-formed UTF-16, no control
// characters except line feed and tab, no bidirectional embedding, override or
// isolate controls, and at least one visible character. Unpaired surrogates
// would become U+FFFD in JSON, so a client before-image built from the JSON
// text could never match the stored annotation again; bidi controls can make
// a shared note display differently from what it says.
void validateText(const QString &text, const QString &path, ModelValidationReport &report)
{
    if (text.size() > 4096)
        issue(report, path, QStringLiteral("must not exceed 4096 UTF-16 code units"));
    if (!text.isValidUtf16())
    {
        issue(report, path, QStringLiteral("must be well-formed UTF-16 without unpaired surrogates"));
        return;
    }
    bool control = false;
    bool bidi = false;
    bool visible = false;
    const QList<uint> codePoints = text.toUcs4();
    for (const uint codePoint : codePoints)
    {
        if ((codePoint < 0x20U && codePoint != 0x09U && codePoint != 0x0AU) ||
            (codePoint >= 0x7FU && codePoint <= 0x9FU))
            control = true;
        if ((codePoint >= 0x202AU && codePoint <= 0x202EU) ||
            (codePoint >= 0x2066U && codePoint <= 0x2069U))
            bidi = true;
        const QChar::Category category = QChar::category(static_cast<char32_t>(codePoint));
        if (!QChar::isSpace(static_cast<char32_t>(codePoint)) &&
            category != QChar::Other_Control && category != QChar::Other_Format &&
            category != QChar::Separator_Space && category != QChar::Separator_Line &&
            category != QChar::Separator_Paragraph)
            visible = true;
    }
    if (control)
        issue(report, path, QStringLiteral("must not contain control characters other than line feed and tab"));
    if (bidi)
        issue(report, path, QStringLiteral("must not contain bidirectional embedding, override or isolate controls"));
    if (!visible)
        issue(report, path, QStringLiteral("must not be blank"));
}

void validateStyle(const MapAnnotationStyle &value, const QString &prefix,
                   ModelValidationReport &report)
{
    switch (value.text_size)
    {
    case MapAnnotationTextSize::Small:
    case MapAnnotationTextSize::Normal:
    case MapAnnotationTextSize::Large:
        break;
    default:
        issue(report, prefix + QStringLiteral(".text_size"), QStringLiteral("unknown text size"));
        break;
    }
}
} // namespace

std::optional<MapAnnotationDomain> mapAnnotationAnchorTargetDomain(const QString &entityType)
{
    for (const AnchorTargetType &target : anchorTargetTypes)
    {
        if (entityType == QLatin1StringView(target.canonical_entity_type))
            return target.domain;
    }
    return std::nullopt;
}

ModelValidationReport validateMapAnnotationAnchor(const MapAnnotationAnchor &value)
{
    ModelValidationReport report;
    validateAnchor(value, QStringLiteral("annotation_anchor"), report);
    return report;
}

ModelValidationReport validateMapAnnotationStyle(const MapAnnotationStyle &value)
{
    ModelValidationReport report;
    validateStyle(value, QStringLiteral("annotation_style"), report);
    return report;
}

ModelValidationReport validateMapAnnotation(const MapAnnotation &value)
{
    ModelValidationReport report;
    if (value.uuid.isNull())
        issue(report, QStringLiteral("annotation.uuid"), QStringLiteral("must not be null"));
    validateText(value.text, QStringLiteral("annotation.text"), report);
    if (!finitePosition(value.position_wgs84))
        issue(report, QStringLiteral("annotation.position_wgs84"), QStringLiteral("invalid WGS84 position"));
    if (value.anchor)
        validateAnchor(*value.anchor, QStringLiteral("annotation.anchor"), report);
    validateStyle(value.style, QStringLiteral("annotation.style"), report);
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
