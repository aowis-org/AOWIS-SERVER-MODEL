#ifndef AOWIS_MODEL_HYDRAULIC_MULTI_SPECIES_RESULTS_H
#define AOWIS_MODEL_HYDRAULIC_MULTI_SPECIES_RESULTS_H

#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QUuid>
#include <QtGlobal>

#include "hydraulic_simulation_diagnostics.h"
#include "hydraulic_simulation_status.h"

// A single species' value at one entity, one timestep. Kept as an
// id-value pair rather than a fixed field per species, since the set of
// species is defined per network by NetworkMultiSpecies::species rather
// than being a fixed AOWIS-wide list.
struct MultiSpeciesResultValue
{
    QUuid species_uuid;
    double concentration = 0.0;
};

struct MultiSpeciesSimulationResultNodeJunction
{
    QString id;
    QUuid uuid;

    QList<MultiSpeciesResultValue> species_values;
};

struct MultiSpeciesSimulationResultNodeReservoir
{
    QString id;
    QUuid uuid;

    QList<MultiSpeciesResultValue> species_values;
};

struct MultiSpeciesSimulationResultNodeTank
{
    QString id;
    QUuid uuid;

    QList<MultiSpeciesResultValue> species_values;
};

struct MultiSpeciesSimulationResultLinkPipe
{
    QString id;
    QUuid uuid;

    QList<MultiSpeciesResultValue> species_values;
};

struct MultiSpeciesSimulationResultLinkPump
{
    QString id;
    QUuid uuid;

    QList<MultiSpeciesResultValue> species_values;
};

struct MultiSpeciesSimulationResultLinkValve
{
    QString id;
    QUuid uuid;

    QList<MultiSpeciesResultValue> species_values;
};

struct MultiSpeciesSimulationResultStatistics
{
    // One mass balance ratio per species, since MSX closes a separate mass
    // balance for each species rather than one balance for the whole run.
    QList<MultiSpeciesResultValue> mass_balance_ratios;
};

struct MultiSpeciesSimulationResult
{
    quint64 time_elapsed_s = 0;
    HydraulicSimulationStatus status;

    QList<MultiSpeciesSimulationResultNodeJunction> nodes_junctions;
    QList<MultiSpeciesSimulationResultNodeReservoir> nodes_reservoirs;
    QList<MultiSpeciesSimulationResultNodeTank> nodes_tanks;

    QList<MultiSpeciesSimulationResultLinkPipe> links_pipes;
    QList<MultiSpeciesSimulationResultLinkPump> links_pumps;
    QList<MultiSpeciesSimulationResultLinkValve> links_valves;

    MultiSpeciesSimulationResultStatistics statistics;
};

enum class MultiSpeciesSimulationResultValidity
{
    NotRun,
    Valid,
    Partial,
    Invalid
};

Q_DECLARE_METATYPE(MultiSpeciesSimulationResultValidity)

struct MultiSpeciesSimulationResultTimeline
{
    HydraulicSimulationStatus status;
    MultiSpeciesSimulationResultValidity validity = MultiSpeciesSimulationResultValidity::NotRun;
    QList<HydraulicSimulationDiagnostic> diagnostics;
    QDateTime simulation_start_utc;
    QList<MultiSpeciesSimulationResult> results;
};

Q_DECLARE_METATYPE(MultiSpeciesSimulationResultTimeline)

#endif // AOWIS_MODEL_HYDRAULIC_MULTI_SPECIES_RESULTS_H
