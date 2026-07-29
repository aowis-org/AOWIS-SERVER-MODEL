#ifndef NETWORK_HYDRAULIC_H
#define NETWORK_HYDRAULIC_H

#include <QList>
#include <QString>
#include <QUuid>

#include "epanet_controls.h"
#include "epanet_curves.h"
#include "epanet_links.h"
#include "epanet_nodes.h"
#include "epanet_options.h"
#include "epanet_types.h"

struct CustomerPoint
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
    // flow_units and pressure_units only define EPANET import/export and solver units.
    long duration_s = 0;
    long hydraulic_timestep_s = 3600;
    long quality_timestep_s = 300;
    long pattern_timestep_s = 3600;
    long pattern_start_s = 0;
    long report_timestep_s = 3600;
    long report_start_s = 0;
    long rule_timestep_s = 360;
    long start_time_of_day_s = 0;
    EpanetReportingStatistic reporting_statistic = EpanetReportingStatistic::Series;

    EpanetHydraulicOptions hydraulic_options;
    EpanetQualityOptions quality_options;
    EpanetReactionOptions reaction_options;
    EpanetEnergyOptions energy_options;
    EpanetReportOptions report_options;

    QList<EpanetTimePattern> patterns;

    QList<TankVolumeCurve> tank_volume_curves;
    QList<PumpCurve> pump_curves;
    QList<PumpEfficiencyCurve> pump_efficiency_curves;
    QList<ValveHeadlossCurve> valve_headloss_curves;
    QList<ValveCharacteristicCurve> valve_characteristic_curves;
    QList<GenericCurve> generic_curves;

    QList<Reservoir> reservoirs;
    QList<Junction> junctions;
    QList<Tank> tanks;

    QList<Pipe> pipes;
    QList<Pump> pumps;
    QList<Valve> valves;

    QList<EpanetSimpleControl> controls;
    QList<EpanetRule> rules;

    QList<EpanetMapLabel> labels;
    EpanetBackdrop backdrop;

    QList<CustomerPoint> customer_points;
};

#endif // NETWORK_HYDRAULIC_H
