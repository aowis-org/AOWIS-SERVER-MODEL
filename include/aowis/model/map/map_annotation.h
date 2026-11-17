#ifndef AOWIS_MODEL_MAP_MAP_ANNOTATION_H
#define AOWIS_MODEL_MAP_MAP_ANNOTATION_H

#include <aowis/model/gis.h>

#include <optional>
#include <QString>
#include <QUuid>

// Domain-neutral, revisioned project presentation values. Storage ownership,
// entity existence and anchor closure are validated by the DVMDB adapter.
enum class MapAnnotationDomain
{
    Distribution,
    Drainage
};

enum class MapAnnotationTextSize
{
    Small,
    Normal,
    Large
};

struct MapAnnotationAnchor
{
    QUuid entity_uuid;
    // Stable MODEL type key, not a translated UI label.
    QString entity_type;
    double offset_east_m = 0.0;
    double offset_north_m = 0.0;
};

struct MapAnnotationStyle
{
    MapAnnotationTextSize text_size = MapAnnotationTextSize::Normal;
    bool show_background = false;
    bool visible = true;
};

struct MapAnnotation
{
    QUuid uuid;
    QString text;
    // Last explicitly placed position; anchored positions are derived at render time.
    CoordinateWGS84 position_wgs84;
    std::optional<MapAnnotationAnchor> anchor;
    MapAnnotationStyle style;
};

// This is only the canonical layer metadata, NOT an inline member list.
// Ordered annotation membership belongs to DVMDB's native layer chunks.
struct MapAnnotationLayer
{
    QUuid uuid;
    MapAnnotationDomain domain = MapAnnotationDomain::Distribution;
    std::optional<QUuid> owner_network_uuid;
};

#endif // AOWIS_MODEL_MAP_MAP_ANNOTATION_H
