#ifndef AOWIS_MODEL_HYDRAULIC_NETWORK_HYDRAULIC_H
#define AOWIS_MODEL_HYDRAULIC_NETWORK_HYDRAULIC_H

#include <QList>
#include <QString>
#include <QUuid>
#include <QtGlobal>

#include "hydraulic_controls.h"
#include "hydraulic_curves.h"
#include "hydraulic_links.h"
#include "hydraulic_map.h"
#include "hydraulic_nodes.h"
#include "hydraulic_simulation_options.h"
#include "hydraulic_types.h"

struct NetworkHydraulicCustomerPoint
{
    QString id;
    QUuid uuid;
};

struct NetworkHydraulic
{
    QString id;
    QUuid uuid;

    QString title_line_1;
    QString title_line_2;
    QString title_line_3;

    // Measurement quantities are stored in the canonical units encoded by their field names.
    // Conversion to other units belongs only at import, export, and presentation boundaries.
    quint64 duration_s = 0;
    quint64 timestep_hydraulic_s = 3600;
    quint64 timestep_quality_s = 300;
    quint64 timestep_pattern_s = 3600;
    quint64 start_pattern_s = 0;
    quint64 timestep_report_s = 3600;
    quint64 start_report_s = 0;
    quint64 timestep_rule_s = 360;
    quint64 start_time_of_day_s = 0;
    HydraulicSimulationReportStatistic report_statistic = HydraulicSimulationReportStatistic::Series;

    HydraulicSolverOptions options_hydraulic;
    WaterQualityReactionOptions options_reaction;
    PumpEnergyOptions options_energy;
    HydraulicSimulationReportOptions options_report;

    QList<HydraulicPatternTime> patterns_time;

    QList<HydraulicCurveTankVolume> curves_tank_volume;
    QList<HydraulicCurvePumpHead> curves_pump_head;
    QList<HydraulicCurvePumpEfficiency> curves_pump_efficiency;
    QList<HydraulicCurveValveHeadloss> curves_valve_headloss;
    QList<HydraulicCurveValveCharacteristic> curves_valve_characteristic;
    QList<HydraulicCurveGeneric> curves_generic;

    QList<HydraulicNodeReservoir> nodes_reservoirs;
    QList<HydraulicNodeJunction> nodes_junctions;
    QList<HydraulicNodeTank> nodes_tanks;

    QList<HydraulicLinkPipe> links_pipes;
    QList<HydraulicLinkPump> links_pumps;
    QList<HydraulicLinkValve> links_valves;

    QList<HydraulicControlSimple> controls_simple;
    QList<HydraulicControlRule> controls_rules;

    QList<HydraulicMapLabel> map_labels;
    HydraulicMapBackdrop map_backdrop;

    QList<NetworkHydraulicCustomerPoint> customer_points;
};

#endif // AOWIS_MODEL_HYDRAULIC_NETWORK_HYDRAULIC_H
