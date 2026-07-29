#ifndef AOWIS_MODEL_HYDRAULIC_NODES_H
#define AOWIS_MODEL_HYDRAULIC_NODES_H

#include <optional>

#include <QList>
#include <QString>
#include <QUuid>

#include "hydraulic_types.h"

struct HydraulicNodeJunctionDemand
{
    QString category_name;
    double base_demand_m3_per_h = 0.0;
    QString pattern_id;

    HydraulicNodeJunctionDemandSourceMethod source_method = HydraulicNodeJunctionDemandSourceMethod::ManualEstimation;
    QString note;
};

struct HydraulicNodeJunction
{
    QString id;
    QUuid uuid;

    double latitude_deg = 0.0;
    double longitude_deg = 0.0;
    std::optional<HydraulicMapPosition> map_position;

    HydraulicNodeElevationInputType elevation_input_type = HydraulicNodeElevationInputType::TotalElevation;
    double elevation_m = 0.0;
    double terrain_elevation_m = 0.0;
    double elevation_offset_m = 0.0;

    QList<HydraulicNodeJunctionDemand> demands;

    double emitter_coefficient_lps_per_m_exponent = 0.0;

    double initial_quality = 0.0;
    HydraulicNodeQualitySource quality_source;

    HydraulicEntityMetadata metadata;
};

struct HydraulicNodeReservoir
{
    QString id;
    QUuid uuid;

    double latitude_deg = 0.0;
    double longitude_deg = 0.0;
    std::optional<HydraulicMapPosition> map_position;

    HydraulicNodeElevationInputType head_input_type = HydraulicNodeElevationInputType::TotalHead;
    double head_m = 0.0;
    double terrain_elevation_m = 0.0;
    double head_offset_m = 0.0;
    QString head_pattern_id;

    double initial_quality = 0.0;
    HydraulicNodeQualitySource quality_source;

    HydraulicEntityMetadata metadata;
};

struct HydraulicNodeTank
{
    QString id;
    QUuid uuid;

    double latitude_deg = 0.0;
    double longitude_deg = 0.0;
    std::optional<HydraulicMapPosition> map_position;

    HydraulicNodeTankElevationInputType elevation_input_type = HydraulicNodeTankElevationInputType::BottomElevation;
    double bottom_elevation_m = 0.0;
    double terrain_elevation_m = 0.0;
    double bottom_offset_m = 0.0;

    double water_level_initial_m = 0.0;
    double water_level_minimum_m = 0.0;
    double water_level_maximum_m = 0.0;

    HydraulicNodeTankGeometryInputType geometry_input_type = HydraulicNodeTankGeometryInputType::Cylindrical;
    double diameter_m = 0.0;
    double cross_section_area_m2 = 0.0;
    double volume_at_maximum_level_m3 = 0.0;
    double minimum_volume_m3 = 0.0;
    QString volume_curve_id;

    bool can_overflow = false;

    double initial_quality = 0.0;
    HydraulicNodeQualitySource quality_source;

    HydraulicNodeTankMixingModel mixing_model = HydraulicNodeTankMixingModel::CompleteMix;
    double mixing_fraction = 1.0;

    bool override_bulk_reaction_coefficient = false;
    double bulk_reaction_coefficient_per_day = 0.0;

    HydraulicEntityMetadata metadata;
};

#endif // AOWIS_MODEL_HYDRAULIC_NODES_H
