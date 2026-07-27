#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_LINKS_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_LINKS_H

#include <optional>
#include <QList>
#include <QString>
#include <QUuid>

#include "epanet_types.h"

struct Pipe
{
    QString id;
    QUuid uuid;

    QString node_id_from;
    QString node_id_to;
    QList<EpanetLinkVertex> vertices;

    double length_calculated_m = 0.0;
    std::optional<double> length_measured_m;

    PipeInitialStatus initial_status = PipeInitialStatus::Open;
    double diameter_mm = 100.0;

    QString material_id;
    double roughness_hw = 130.0;
    double roughness_dw_mm = 0.1;
    double roughness_cm = 0.013;
    double minor_loss = 0.0;

    bool override_reaction_coefficients = false;
    double bulk_reaction_coefficient_per_day = 0.0;
    double wall_reaction_coefficient_m_per_day = 0.0;

    double leak_area_mm2_per_100m = 0.0;
    double leak_expansion_mm2_per_m_head = 0.0;

    EpanetObjectMetadata metadata;
};

struct Pump
{
    QString id;
    QUuid uuid;

    QString node_id_from;
    QString node_id_to;
    QList<EpanetLinkVertex> vertices;

    PumpDefinitionType definition_type = PumpDefinitionType::ConstantPower;
    double constant_power_kw = 0.0;
    QString head_curve_id;

    double initial_speed = 1.0;
    PumpInitialStatus initial_status = PumpInitialStatus::On;
    QString speed_pattern_id;

    PumpControlType control_type = PumpControlType::None;

    PumpEfficiencyInputType efficiency_input_type = PumpEfficiencyInputType::Global;
    double constant_efficiency_percent = 75.0;
    QString efficiency_curve_id;

    PumpEnergyPriceInputType energy_price_input_type = PumpEnergyPriceInputType::Global;
    double energy_price_per_kwh = 0.0;
    QString price_pattern_id;

    EpanetObjectMetadata metadata;
};

struct Valve
{
    QString id;
    QUuid uuid;

    QString node_id_from;
    QString node_id_to;
    QList<EpanetLinkVertex> vertices;

    ValveType type = ValveType::PRV;

    // PRV, PSV, and PBV: pressure in meters of water.
    // FCV: flow in m3/h.
    // TCV: dimensionless loss coefficient.
    // PCV: position in percent.
    // GPV uses setting_curve_id instead.
    double setting = 0.0;
    QString setting_curve_id;

    ValveInitialStatus initial_status = ValveInitialStatus::Active;
    double diameter_mm = 0.0;
    double minor_loss = 0.0;

    EpanetObjectMetadata metadata;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_LINKS_H
