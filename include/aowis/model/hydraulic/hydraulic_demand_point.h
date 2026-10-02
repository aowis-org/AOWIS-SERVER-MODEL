#ifndef AOWIS_MODEL_HYDRAULIC_DEMAND_POINT_H
#define AOWIS_MODEL_HYDRAULIC_DEMAND_POINT_H

#include <QList>
#include <QString>
#include <QUuid>

#include <optional>

#include "../entity.h"
#include "../gis.h"
#include "../measurement/water_meter.h"
#include "hydraulic_demands.h"

enum class HydraulicDemandPointAttachmentType
{
    None,
    Pipe,
    Junction
};

// Logical attachment of a demand point to the modeled hydraulic network.
// A pipe attachment is a normalized position along the complete pipe geometry:
// 0.0 is the pipe's from-node and 1.0 is its to-node.
// This is not a hydraulic node. If the attachment location must become an
// explicit hydraulic state point, convert it to a junction in the editable model.
struct HydraulicDemandPointAttachment
{
    HydraulicDemandPointAttachmentType type = HydraulicDemandPointAttachmentType::None;

    QUuid pipe_uuid;
    double pipe_position = 0.0;

    QUuid junction_uuid;
};

// A geographically located demand that is intentionally not part of the hydraulic
// node/link topology. The demand-point coordinate and the network attachment are
// independent: the coordinate says where the demand is, while the attachment says
// where it is supplied from the modeled network.
struct HydraulicDemandPoint
{
    QString id;
    QUuid uuid;

    CoordinateWGS84 coordinate_wgs84;

    HydraulicDemandPointAttachment attachment;
    QList<HydraulicDemand> demands;
    std::optional<WaterMeter> meter;

    EntityMetadata metadata;
};

#endif // AOWIS_MODEL_HYDRAULIC_DEMAND_POINT_H
