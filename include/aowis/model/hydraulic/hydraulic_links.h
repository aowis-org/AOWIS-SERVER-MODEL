#ifndef AOWIS_MODEL_HYDRAULIC_LINKS_H
#define AOWIS_MODEL_HYDRAULIC_LINKS_H

#include <optional>

#include <QList>
#include <QString>
#include <QUuid>

#include "hydraulic_types.h"

struct HydraulicLinkPipe
{
    QString id;
    QUuid uuid;

    QUuid node_uuid_from;
    QUuid node_uuid_to;
    QList<HydraulicLinkVertex> vertices;

    double length_calculated_m = 0.0;
    std::optional<double> length_measured_m;

    HydraulicLinkPipeInitialStatus initial_status = HydraulicLinkPipeInitialStatus::Open;
    double diameter_mm = 100.0;

    QString material_id;
    double roughness_hazen_williams = 130.0;
    double roughness_darcy_weisbach_mm = 0.1;
    double roughness_chezy_manning = 0.013;
    double minor_loss_coefficient = 0.0;

    bool override_bulk_reaction = false;
    WaterQualityBulkReaction bulk_reaction;
    bool override_wall_reaction = false;
    WaterQualityWallReaction wall_reaction;

    double leak_area_mm2_per_100m = 0.0;
    double leak_area_expansion_per_pressure_head_mm2_per_m = 0.0;

    HydraulicEntityMetadata metadata;
};

struct HydraulicLinkPump
{
    QString id;
    QUuid uuid;

    QUuid node_uuid_from;
    QUuid node_uuid_to;
    QList<HydraulicLinkVertex> vertices;

    // One-point, three-point, multi-point, and library definitions use
    // head_curve_uuid. The curve point count determines how a simulation
    // backend interprets the pump curve.
    HydraulicLinkPumpDefinitionType definition_type = HydraulicLinkPumpDefinitionType::ConstantPower;
    double constant_power_kw = 0.0;
    QUuid head_curve_uuid;

    double initial_speed_ratio = 1.0;
    HydraulicLinkPumpInitialStatus initial_status = HydraulicLinkPumpInitialStatus::On;
    QUuid speed_pattern_uuid;

    HydraulicLinkPumpEfficiencyInputType efficiency_input_type = HydraulicLinkPumpEfficiencyInputType::Global;
    double constant_efficiency_percent = 75.0;
    QUuid efficiency_curve_uuid;

    HydraulicLinkPumpEnergyPriceInputType energy_price_input_type = HydraulicLinkPumpEnergyPriceInputType::Global;
    double energy_price_per_kw_h = 0.0;
    QUuid price_pattern_uuid;

    HydraulicEntityMetadata metadata;
};

struct HydraulicLinkValve
{
    QString id;
    QUuid uuid;

    QUuid node_uuid_from;
    QUuid node_uuid_to;
    QList<HydraulicLinkVertex> vertices;

    HydraulicLinkValveType type = HydraulicLinkValveType::PRV;

    // Only the setting field applicable to type is used. Keeping the physical
    // quantities as separate fields makes their units explicit in the Model.
    double setting_pressure_head_m = 0.0;
    double setting_flow_m3_per_h = 0.0;
    double setting_loss_coefficient = 0.0;
    double setting_position_percent = 0.0;

    // GPV uses a head-loss curve. PCV may use a valve-characteristic curve.
    QUuid head_loss_curve_uuid;
    QUuid characteristic_curve_uuid;

    HydraulicLinkValveInitialStatus initial_status = HydraulicLinkValveInitialStatus::Active;
    double diameter_mm = 0.0;
    double minor_loss_coefficient = 0.0;

    HydraulicEntityMetadata metadata;
};

#endif // AOWIS_MODEL_HYDRAULIC_LINKS_H
