#ifndef AOWIS_MODEL_HYDRAULIC_DEMAND_POINT_H
#define AOWIS_MODEL_HYDRAULIC_DEMAND_POINT_H

#include <QList>
#include <QString>
#include <QUuid>

#include "../entity.h"
#include "../gis.h"
#include "../measurement/water_meter.h"
#include "hydraulic_demands.h"

// A solver or adapter can distribute a demand point over one or more hydraulic junctions.
// Fractions for one demand point are expected to sum to 1.0 when allocations are present.
struct HydraulicDemandPointAllocation
{
    QUuid junction_uuid;
    double fraction = 1.0;
};

// A geographically located demand that is intentionally not part of the hydraulic
// node/link topology. If connection hydraulics matter, model them explicitly with
// junctions and links instead of adding hydraulic connection properties here.
struct HydraulicDemandPoint
{
    QString id;
    QUuid uuid;

    CoordinateWGS84 coordinate_wgs84;

    QList<HydraulicDemand> demands;
    QList<HydraulicDemandPointAllocation> allocations;
    QList<WaterMeter> meters;

    EntityMetadata metadata;
};

#endif // AOWIS_MODEL_HYDRAULIC_DEMAND_POINT_H
