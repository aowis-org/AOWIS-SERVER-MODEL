#ifndef AOWIS_MODEL_WATER_QUALITY_SIMULATION_RESULTS_H
#define AOWIS_MODEL_WATER_QUALITY_SIMULATION_RESULTS_H

#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QUuid>
#include <QtGlobal>

#include "hydraulic_simulation_diagnostics.h"
#include "hydraulic_simulation_status.h"
#include "hydraulic_types.h"

struct WaterQualitySimulationResultNodeJunction
{
    QString id;
    QUuid uuid;

    double chemical_concentration_mg_per_l = 0.0;
    double water_age_h = 0.0;
    double source_trace_percent = 0.0;
    double source_mass_flow_mg_per_min = 0.0;
};

struct WaterQualitySimulationResultNodeReservoir
{
    QString id;
    QUuid uuid;

    double chemical_concentration_mg_per_l = 0.0;
    double water_age_h = 0.0;
    double source_trace_percent = 0.0;
    double source_mass_flow_mg_per_min = 0.0;
};

struct WaterQualitySimulationResultNodeTank
{
    QString id;
    QUuid uuid;

    double chemical_concentration_mg_per_l = 0.0;
    double water_age_h = 0.0;
    double source_trace_percent = 0.0;
    double source_mass_flow_mg_per_min = 0.0;
};

struct WaterQualitySimulationResultLinkPipe
{
    QString id;
    QUuid uuid;

    double chemical_concentration_mg_per_l = 0.0;
    double water_age_h = 0.0;
    double source_trace_percent = 0.0;
};

struct WaterQualitySimulationResultLinkPump
{
    QString id;
    QUuid uuid;

    double chemical_concentration_mg_per_l = 0.0;
    double water_age_h = 0.0;
    double source_trace_percent = 0.0;
};

struct WaterQualitySimulationResultLinkValve
{
    QString id;
    QUuid uuid;

    double chemical_concentration_mg_per_l = 0.0;
    double water_age_h = 0.0;
    double source_trace_percent = 0.0;
};

struct WaterQualitySimulationResultStatistics
{
    double mass_balance_ratio = 0.0;
};

struct WaterQualitySimulationResult
{
    quint64 time_elapsed_s = 0;
    HydraulicSimulationStatus status;

    QList<WaterQualitySimulationResultNodeJunction> nodes_junctions;
    QList<WaterQualitySimulationResultNodeReservoir> nodes_reservoirs;
    QList<WaterQualitySimulationResultNodeTank> nodes_tanks;

    QList<WaterQualitySimulationResultLinkPipe> links_pipes;
    QList<WaterQualitySimulationResultLinkPump> links_pumps;
    QList<WaterQualitySimulationResultLinkValve> links_valves;

    WaterQualitySimulationResultStatistics statistics;
};

enum class WaterQualitySimulationResultValidity
{
    NotRun,
    Valid,
    Partial,
    Invalid
};

Q_DECLARE_METATYPE(WaterQualitySimulationResultValidity)

struct WaterQualitySimulationResultTimeline
{
    WaterQualityAnalysisType analysis = WaterQualityAnalysisType::None;
    HydraulicSimulationStatus status;
    WaterQualitySimulationResultValidity validity = WaterQualitySimulationResultValidity::NotRun;
    QList<HydraulicSimulationDiagnostic> diagnostics;
    QDateTime simulation_start_utc;
    QList<WaterQualitySimulationResult> results;
};

Q_DECLARE_METATYPE(WaterQualitySimulationResultTimeline)

#endif // AOWIS_MODEL_WATER_QUALITY_SIMULATION_RESULTS_H
