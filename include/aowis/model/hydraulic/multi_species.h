#ifndef AOWIS_MODEL_HYDRAULIC_MULTI_SPECIES_H
#define AOWIS_MODEL_HYDRAULIC_MULTI_SPECIES_H

#include <QList>
#include <QString>
#include <QUuid>
#include <QtGlobal>

#include "hydraulic_types.h"

// The multi-species reaction model (species, terms, and reaction expressions)
// is written by the user as literal math expressions evaluated by the MSX
// solver itself. Unlike other Model fields, those expressions cannot be
// silently canonicalized into one fixed SI unit at the AOWIS boundary,
// because the unit convention they were written under is part of their
// meaning. area_units and rate_units below are therefore kept as explicit,
// persisted fields -- the one deliberate exception to the "store everything
// in the unit encoded by the field name" rule used elsewhere in the Model.

enum class MultiSpeciesSpeciesType
{
    Bulk,
    Wall
};

enum class MultiSpeciesUnits
{
    Milligrams,
    Micrograms,
    Moles
};

enum class MultiSpeciesAreaUnits
{
    SquareFeet,
    SquareMetres,
    SquareCentimetres
};

enum class MultiSpeciesRateUnits
{
    Seconds,
    Minutes,
    Hours,
    Days
};

enum class MultiSpeciesSolverMethod
{
    Euler,
    RungeKutta5,
    Rosenbrock2
};

enum class MultiSpeciesCouplingMethod
{
    Full,
    None
};

enum class MultiSpeciesReactionLocation
{
    Pipe,
    Tank
};

enum class MultiSpeciesReactionExpressionType
{
    Rate,
    Formula,
    Equilibrium
};

enum class MultiSpeciesSourceType
{
    Concentration,
    Mass,
    FlowPaced,
    Setpoint
};

struct MultiSpeciesSpecies
{
    QString id;
    QUuid uuid;

    MultiSpeciesSpeciesType type = MultiSpeciesSpeciesType::Bulk;
    MultiSpeciesUnits units = MultiSpeciesUnits::Milligrams;

    // A zero tolerance means the network-wide default in MultiSpeciesOptions
    // applies to this species instead of a per-species override.
    double absolute_tolerance = 0.0;
    double relative_tolerance = 0.0;

    QString note;
};

struct MultiSpeciesConstant
{
    QString id;
    QUuid uuid;

    double value = 0.0;
};

// A named coefficient whose value may vary by pipe or tank. default_value
// applies to any pipe or tank without an explicit override below.
struct MultiSpeciesParameter
{
    QString id;
    QUuid uuid;

    double default_value = 0.0;
};

struct MultiSpeciesParameterOverridePipe
{
    QUuid pipe_uuid;
    QUuid parameter_uuid;
    double value = 0.0;
};

struct MultiSpeciesParameterOverrideTank
{
    QUuid tank_uuid;
    QUuid parameter_uuid;
    double value = 0.0;
};

// A named intermediate expression usable inside reaction expressions, so a
// shared sub-expression only has to be written once.
struct MultiSpeciesTerm
{
    QString id;
    QUuid uuid;

    QString expression;
};

// One reaction expression per (species, location). Per-pipe/per-tank
// variation is expressed through MultiSpeciesParameter overrides referenced
// symbolically from expression, not by repeating the expression itself.
struct MultiSpeciesReaction
{
    QUuid uuid;

    QUuid species_uuid;
    MultiSpeciesReactionLocation location = MultiSpeciesReactionLocation::Pipe;
    MultiSpeciesReactionExpressionType expression_type = MultiSpeciesReactionExpressionType::Rate;
    QString expression;
};

// Species dosing patterns use their own namespace, independent of the
// hydraulic HydraulicPatternTime patterns used for demand/head/speed/price.
struct MultiSpeciesPattern
{
    QString id;
    QUuid uuid;

    QList<double> multipliers;
    QString comment;
};

struct MultiSpeciesGlobalInitialQuality
{
    QUuid species_uuid;
    double concentration = 0.0;
};

struct MultiSpeciesNodeInitialQuality
{
    QUuid node_uuid;
    QUuid species_uuid;
    double concentration = 0.0;
};

// Unlike core EPANET water quality, MSX also allows an explicit initial
// condition on pipes (their contents are not assumed to start at zero or at
// an upstream node's value).
struct MultiSpeciesPipeInitialQuality
{
    QUuid pipe_uuid;
    QUuid species_uuid;
    double concentration = 0.0;
};

struct MultiSpeciesNodeSource
{
    QUuid node_uuid;
    QUuid species_uuid;

    MultiSpeciesSourceType type = MultiSpeciesSourceType::Concentration;

    // Concentration, flow-paced, and setpoint sources use a concentration;
    // mass sources use an absolute mass flow. Which one applies follows type,
    // mirroring HydraulicNodeQualitySource's split of the same ambiguity.
    double concentration = 0.0;
    double mass_flow_per_min = 0.0;

    QUuid pattern_uuid;
};

struct MultiSpeciesOptions
{
    MultiSpeciesAreaUnits area_units = MultiSpeciesAreaUnits::SquareFeet;
    MultiSpeciesRateUnits rate_units = MultiSpeciesRateUnits::Days;

    MultiSpeciesSolverMethod solver_method = MultiSpeciesSolverMethod::RungeKutta5;
    MultiSpeciesCouplingMethod coupling_method = MultiSpeciesCouplingMethod::Full;

    quint64 timestep_s = 300;

    // Network-wide default tolerances, used by any species that does not
    // define its own absolute_tolerance/relative_tolerance.
    double default_absolute_tolerance = 0.01;
    double default_relative_tolerance = 0.001;
};

// Execution-time selection of which species to simulate for one run. Not
// network state -- carried only by EpanetRunRequest::multi_species_run, the
// same way WaterQualitySolverOptions is carried only by
// EpanetRunRequest::quality_runs. An empty list means every species in
// NetworkMultiSpecies::species is simulated; a non-empty list restricts this
// run to just those species, letting one network's reaction library serve
// multiple narrower runs.
struct MultiSpeciesRunOptions
{
    QList<QUuid> species_uuids;
};

// The full multi-species reaction model attached to a NetworkHydraulic. An
// empty species list means no multi-species model is defined for this
// network, mirroring how an empty patterns_time list means no time patterns
// are defined.
struct NetworkMultiSpecies
{
    MultiSpeciesOptions options;

    QList<MultiSpeciesSpecies> species;
    QList<MultiSpeciesConstant> constants;
    QList<MultiSpeciesParameter> parameters;
    QList<MultiSpeciesParameterOverridePipe> parameter_overrides_pipes;
    QList<MultiSpeciesParameterOverrideTank> parameter_overrides_tanks;
    QList<MultiSpeciesTerm> terms;
    QList<MultiSpeciesReaction> reactions;
    QList<MultiSpeciesPattern> patterns;

    QList<MultiSpeciesGlobalInitialQuality> initial_quality_global;
    QList<MultiSpeciesNodeInitialQuality> initial_quality_nodes;
    QList<MultiSpeciesPipeInitialQuality> initial_quality_pipes;

    QList<MultiSpeciesNodeSource> sources;
};

#endif // AOWIS_MODEL_HYDRAULIC_MULTI_SPECIES_H
