#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_NODES_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_NODES_H

#include <optional>

#include <QList>
#include <QString>
#include <QUuid>

#include "epanet_types.h"

struct EpanetNodeJunctionDemand
{
    QString category_name;
    double base_demand_m3_per_h = 0.0;
    QString pattern_id;

    EpanetNodeJunctionDemandSourceMethod source_method = EpanetNodeJunctionDemandSourceMethod::ManualEstimation;
    QString note;
};

struct EpanetNodeJunction
{
    QString id;
    QUuid uuid;

    double latitude_deg = 0.0;
    double longitude_deg = 0.0;
    std::optional<EpanetMapPosition> map_position;

    EpanetNodeElevationInputType elevation_input_type = EpanetNodeElevationInputType::TotalElevation;
    double elevation_m = 0.0;
    double terrain_elevation_m = 0.0;
    double elevation_offset_m = 0.0;

    QList<EpanetNodeJunctionDemand> demands;

    double emitter_coefficient_lps_per_m_exponent = 0.0;

    double initial_quality = 0.0;
    EpanetNodeQualitySource quality_source;

    EpanetEntityMetadata metadata;
};

struct EpanetNodeReservoir
{
    QString id;
    QUuid uuid;

    double latitude_deg = 0.0;
    double longitude_deg = 0.0;
    std::optional<EpanetMapPosition> map_position;

    EpanetNodeElevationInputType head_input_type = EpanetNodeElevationInputType::TotalHead;
    double head_m = 0.0;
    double terrain_elevation_m = 0.0;
    double head_offset_m = 0.0;
    QString head_pattern_id;

    double initial_quality = 0.0;
    EpanetNodeQualitySource quality_source;

    EpanetEntityMetadata metadata;
};

struct EpanetNodeTank
{
    QString id;
    QUuid uuid;

    double latitude_deg = 0.0;
    double longitude_deg = 0.0;
    std::optional<EpanetMapPosition> map_position;

    EpanetNodeTankElevationInputType elevation_input_type = EpanetNodeTankElevationInputType::BottomElevation;
    double bottom_elevation_m = 0.0;
    double terrain_elevation_m = 0.0;
    double bottom_offset_m = 0.0;

    double water_level_initial_m = 0.0;
    double water_level_minimum_m = 0.0;
    double water_level_maximum_m = 0.0;

    EpanetNodeTankGeometryInputType geometry_input_type = EpanetNodeTankGeometryInputType::Cylindrical;
    double diameter_m = 0.0;
    double cross_section_area_m2 = 0.0;
    double volume_at_maximum_level_m3 = 0.0;
    double minimum_volume_m3 = 0.0;
    QString volume_curve_id;

    bool can_overflow = false;

    double initial_quality = 0.0;
    EpanetNodeQualitySource quality_source;

    EpanetNodeTankMixingModel mixing_model = EpanetNodeTankMixingModel::CompleteMix;
    double mixing_fraction = 1.0;

    bool override_bulk_reaction_coefficient = false;
    double bulk_reaction_coefficient_per_day = 0.0;

    EpanetEntityMetadata metadata;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_NODES_H
