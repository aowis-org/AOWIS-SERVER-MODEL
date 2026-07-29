#ifndef AOWIS_MODEL_HYDRAULIC_NETWORK_HYDRAULIC_H
#define AOWIS_MODEL_HYDRAULIC_NETWORK_HYDRAULIC_H

#include <QList>
#include <QString>
#include <QUuid>

#include "epanet_controls.h"
#include "epanet_curves.h"
#include "epanet_links.h"
#include "epanet_nodes.h"
#include "epanet_options.h"
#include "epanet_types.h"

struct NetworkHydraulicCustomerPoint
{
    QString id;
    QUuid uuid;
};

struct NetworkHydraulic
{
    QUuid uuid;

    QString title_line_1;
    QString title_line_2;
    QString title_line_3;

    // The AOWIS model stores measurement quantities in canonical SI-based engineering units.
    // options_hydraulic.flow_units and options_hydraulic.pressure_units only define EPANET
    // import/export and solver units.
    long duration_s = 0;
    long timestep_hydraulic_s = 3600;
    long timestep_quality_s = 300;
    long timestep_pattern_s = 3600;
    long start_pattern_s = 0;
    long timestep_report_s = 3600;
    long start_report_s = 0;
    long timestep_rule_s = 360;
    long start_time_of_day_s = 0;
    EpanetOptionsReportStatistic report_statistic = EpanetOptionsReportStatistic::Series;

    EpanetOptionsHydraulic options_hydraulic;
    EpanetOptionsQuality options_quality;
    EpanetOptionsReaction options_reaction;
    EpanetOptionsEnergy options_energy;
    EpanetOptionsReport options_report;

    QList<EpanetPatternTime> patterns_time;

    QList<EpanetCurveTankVolume> curves_tank_volume;
    QList<EpanetCurvePumpHead> curves_pump_head;
    QList<EpanetCurvePumpEfficiency> curves_pump_efficiency;
    QList<EpanetCurveValveHeadloss> curves_valve_headloss;
    QList<EpanetCurveValveCharacteristic> curves_valve_characteristic;
    QList<EpanetCurveGeneric> curves_generic;

    QList<EpanetNodeReservoir> nodes_reservoirs;
    QList<EpanetNodeJunction> nodes_junctions;
    QList<EpanetNodeTank> nodes_tanks;

    QList<EpanetLinkPipe> links_pipes;
    QList<EpanetLinkPump> links_pumps;
    QList<EpanetLinkValve> links_valves;

    QList<EpanetControlSimple> controls_simple;
    QList<EpanetControlRule> controls_rules;

    QList<EpanetMapLabel> map_labels;
    EpanetMapBackdrop map_backdrop;

    QList<NetworkHydraulicCustomerPoint> customer_points;
};

#endif // AOWIS_MODEL_HYDRAULIC_NETWORK_HYDRAULIC_H
