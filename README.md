# AOWIS Server Model

Shared C++ domain model for [AOWIS](https://aowis.org) applications, services, and infrastructure components.

This repository provides common data structures and enums for representing AOWIS domain data. It currently covers projects, revisions, GIS coordinates, hydraulic networks, solver-neutral simulation configuration, and hydraulic simulation results. Electrical model structures are planned under the existing `electric` directory.

The repository does not implement the AOWIS server, persistence, REST APIs, user interfaces, or a hydraulic simulation engine. Simulation engines and file-format adapters consume and translate the shared model defined here.

## Specification and Model

The AOWIS wiki defines the normative domain rules and terminology. This repository defines the corresponding concrete C++ representation.

Both must evolve together. A conflict between the wiki specification and the C++ model is considered a defect and must be resolved explicitly rather than allowing either representation to silently override the other.

Relevant measurement specifications:

- [Measurement: Units](https://aowis.org/index.php/Measurement:Units)
- [Measurement: Unit Identifiers](https://aowis.org/index.php/Measurement:Unit_Identifiers)
- [Unified Code for Units of Measure (UCUM)](https://ucum.org)

## Measurement and Unit Policy

AOWIS uses canonical SI-based engineering units.

- Each measurement quantity is assigned exactly one canonical unit.
- Each canonical unit is identified by its UCUM unit identifier.
- The measurement quantity identifier defines what is measured; the unit identifier defines the unit in which it is expressed.
- Fixed-unit C++ fields should include the unit in the field name where this improves clarity, for example `diameter_mm`, `flow_m3_per_h`, or `pressure_head_m`.
- Alternative input, output, display, report, and solver units must be converted at the applicable system boundary.
- Multiple authoritative representations of the same quantity in different units must not be stored in the model.
- Dynamically represented or serialized unit identifiers must conform to UCUM.

The canonical units and quantity identifiers are defined by the AOWIS measurement specification and must remain consistent with the fields in this repository.

## Repository Structure

```text
include/aowis/model/
├── electric/          Reserved for electrical domain models
├── hydraulic/        Hydraulic network and simulation-domain models
├── gis.h              Geographic and projected coordinate structures
├── project.h          Project metadata and lifecycle state
└── revision.h         Project revision metadata and lifecycle state
```

The public headers under `include/aowis/model` form the consumable model API.

## Solver-Neutral Boundary

The public model uses hydraulic-domain terminology rather than the name of any specific simulation engine.

- Network entities use names such as `HydraulicNodeJunction`, `HydraulicLinkPipe`, and `HydraulicCurvePumpHead`.
- Solver configuration uses names such as `HydraulicSolverOptions`, `WaterQualitySolverOptions`, and `HydraulicSimulationReportOptions`.
- Results and diagnostics use `HydraulicSimulationResult*` and `HydraulicSimulationStatus*`.
- Backend-specific operation names, error codes, messages, and report commands are isolated in neutral `backend_*` fields.
- Adapters are responsible for mapping backend constants, numeric status values, API calls, units, and syntax to the shared model.

This allows the same AOWIS model to be consumed by the current simulation backend, a future in-house solver, or another compatible engine without propagating the backend name through the rest of AOWIS.

## Requirements

- CMake 3.21 or newer
- C++20
- Qt 6 Core

## CMake Integration

Add the repository to the consuming project, for example as a Git submodule:

```bash
git submodule add https://github.com/aowis-org/AOWIS-SERVER-MODEL.git external/AOWIS-SERVER-MODEL
git submodule update --init --recursive
```

Add the model from the consuming `CMakeLists.txt`:

```cmake
add_subdirectory(external/AOWIS-SERVER-MODEL)

target_link_libraries(your-target PRIVATE AOWIS::Model)
```

`AOWIS::ServerModel` is available as an equivalent target alias.

Headers can then be included directly:

```cpp
#include <aowis/model/project.h>
#include <aowis/model/gis.h>
#include <aowis/model/hydraulic/network_hydraulic.h>
#include <aowis/model/hydraulic/hydraulic_simulation_results.h>
```

## Installation

The model headers and exported CMake targets can also be installed:

```bash
cmake -S . -B build
cmake --build build
cmake --install build --prefix /desired/install/prefix
```

## Design Principles

- Define shared domain concepts once.
- Keep the model independent of user interfaces, databases, transport protocols, file formats, and simulation-engine wrappers.
- Use explicit field names and types whose semantics do not depend on undocumented context.
- Use one authoritative representation for each value.
- Keep external-system-specific conversion and adaptation at system boundaries.
- Prefer domain terminology over implementation-specific terminology where the two differ.
- Do not encode backend numeric constants as values of shared domain enums.

## Naming Convention

Names are ordered from the broad shared group to the specific concept so related identifiers remain adjacent in autocomplete results.

- Hydraulic topology types use the `HydraulicNode*`, `HydraulicLink*`, and `HydraulicCurve*` groups.
- Hydraulic controls use the `HydraulicControl*` group.
- Simulation results and statuses use the `HydraulicSimulationResult*` and `HydraulicSimulationStatus*` groups.
- Configuration types use their actual domain responsibility, for example `HydraulicSolverOptions`, `WaterQualityReactionOptions`, and `PumpEnergyOptions`.
- Related member names use a shared snake_case prefix followed by the more specific concept.
- Collection members keep the shared group prefix and use a plural specific name.
- External identifiers whose spelling is defined by an external API or serialization format remain isolated inside adapters or explicit `backend_*` diagnostic fields.

For example:

```cpp
HydraulicSolverOptions options_hydraulic;
QList<HydraulicNodeJunction> nodes_junctions;
QList<HydraulicLinkPipe> links_pipes;
QList<HydraulicCurvePumpHead> curves_pump_head;
```

`WaterQualitySolverOptions` describes one requested quality analysis rather than persistent network state. Solver execution requests carry those options alongside a `NetworkHydraulic`; the network itself stores quality inputs, sources, mixing, reactions, and shared simulation timing.

## Contributing

Changes to measurement quantities, canonical units, unit-bearing field names, or serialization semantics must be reviewed against the AOWIS measurement specification.

A model change that introduces or modifies a measured quantity should identify:

- the measurement quantity;
- its canonical SI-based engineering unit;
- its canonical UCUM unit identifier;
- the corresponding C++ field name and type;
- any required conversion at external-system boundaries;
- any corresponding wiki specification change.

Changes to the wiki specification that affect the concrete data model should be accompanied by the corresponding model update, or by a clearly documented follow-up task.

## License

Licensed under the Apache License 2.0. See [LICENSE](LICENSE).
