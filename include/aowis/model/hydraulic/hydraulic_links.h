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

    bool override_reaction_coefficients = false;
    double bulk_reaction_coefficient_per_day = 0.0;
    double wall_reaction_coefficient_m_per_day = 0.0;

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

    // Describes the pump's configured control family for presentation. The
    // executable control logic is stored in NetworkHydraulic::controls_simple
    // and NetworkHydraulic::controls_rules, keyed by this pump's UUID.
    HydraulicLinkPumpControlType control_type = HydraulicLinkPumpControlType::None;

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

    // PRV, PSV, and PBV: pressure in meters of water.
    // FCV: flow in m3/h.
    // TCV: dimensionless loss coefficient.
    // PCV: position in percent.
    // GPV uses setting_curve_uuid for its head-loss curve.
    // PCV uses setting_curve_uuid for its valve-characteristic curve.
    double setting = 0.0;
    QUuid setting_curve_uuid;

    HydraulicLinkValveInitialStatus initial_status = HydraulicLinkValveInitialStatus::Active;
    double diameter_mm = 0.0;
    double minor_loss_coefficient = 0.0;

    HydraulicEntityMetadata metadata;
};

#endif // AOWIS_MODEL_HYDRAULIC_LINKS_H
