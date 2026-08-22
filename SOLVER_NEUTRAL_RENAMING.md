# Solver-Neutral Model Renaming

The hydraulic model previously exposed EPANET as a prefix throughout its public API. This made a simulation-engine implementation detail part of the shared AOWIS domain vocabulary. The public model is now solver-neutral.

## Main Rename Groups

| Previous public group | New public group |
|---|---|
| `EpanetNode*` | `HydraulicNode*` |
| `EpanetLink*` | `HydraulicLink*` |
| `EpanetCurve*` | `HydraulicCurve*` |
| `EpanetControl*` | `HydraulicControl*` |
| `EpanetResult*` | `HydraulicSimulationResult*` |
| `EpanetStatus*` | `HydraulicSimulationStatus*` |
| `EpanetOptions*` | Responsibility-specific option types |
| `EpanetMap*` | `HydraulicMap*` |

Public header filenames were renamed in the same way. No compatibility aliases using the previous names are provided, because such aliases would continue propagating the backend name into new code.

## Option Types

The previous option prefix did not describe the responsibility of each type. These types now use domain-specific names:

| Previous type | New type |
|---|---|
| `EpanetOptionsHydraulic` | `HydraulicSolverOptions` |
| `EpanetOptionsQuality` | `WaterQualitySolverOptions` |
| `EpanetOptionsReaction` | `WaterQualityReactionOptions` |
| `EpanetOptionsEnergy` | `PumpEnergyOptions` |
| `EpanetOptionsReport` | `HydraulicSimulationReportOptions` |

The related enums were similarly renamed, for example `HydraulicFlowUnit`, `HydraulicPressureUnit`, `HydraulicHeadlossFormula`, and `WaterQualityAnalysisType`. Native solver-unit selectors are not part of `HydraulicSolverOptions`; canonical units are encoded by measurement field names, while conversion belongs at import, export, and presentation boundaries.

## Backend-Specific Information

The following concepts are genuinely backend-specific and therefore cannot be represented as shared domain names without either losing diagnostics or leaking backend API terminology:

- backend API function names;
- backend numeric error codes;
- backend-native error messages;
- backend-specific report commands;
- backend numeric constants used for link or pump states;
- exact imported rule syntax.

They are handled as follows:

- `HydraulicSimulationStatus::backend_name` identifies the active backend when needed.
- `backend_error_code`, `backend_operation`, and `message_backend` preserve native diagnostics without defining backend-specific public enums.
- `HydraulicSimulationReportOptions::backend_commands` isolates report commands that only a selected backend understands.
- Shared result enums use semantic values without backend numeric assignments. Adapters must map native constants explicitly.
- Imported control rules must be parsed into the structured shared rule representation. Backend-specific textual formatting is not part of the shared model.

## Required Consumer Changes

Consumers must update includes and public type names. Simulation wrappers must additionally map their native API operations and numeric constants to the generic status and result enums. Database and serialization code must update persisted type or field names only where those C++ names are used directly as schema or serialization keys.

## Additional Breaking Field Changes

| Previous field | New field |
|---|---|
| `additional_commands` | `backend_commands` |
| Generic hydraulic map `x`/`y` positions and backdrop unit selection | Canonical WGS84 coordinates; generated EPANET backdrop data uses degrees |
| `epanet_error_code` | `backend_error_code` |
| `message_epanet` | `message_backend` |
| `HydraulicSimulationResultLinkPipe::setting` | Formula-specific `roughness_hazen_williams`, `roughness_darcy_weisbach_mm`, or `roughness_chezy_manning` |

`HydraulicSolverOptions::flow_unit` and `HydraulicSolverOptions::pressure_unit` were removed. Hydraulic quantities use the canonical units encoded by their field names; external unit selection belongs at import, export, and presentation boundaries.

`HydraulicSimulationStatusOperation` no longer reproduces native API function names as enum values. It contains backend-neutral operation categories. The exact native function or operation name belongs in `HydraulicSimulationStatus::backend_operation`.

## Concepts Still Requiring a Domain Decision

The names are now generic, but several structures still reflect capabilities or conventions of the current backend. They should be reviewed before treating them as permanent cross-backend contracts:

- algorithm controls such as `maximum_trials`, `check_frequency`, `maximum_check`, `damping_limit`, and unbalanced-network handling;
- flow-unit and pressure-unit enum sets used at import/export boundaries, which may be better represented by UCUM identifiers;
- one-point and three-point pump-curve definition modes;
- text-report page, status, selection, threshold, and precision behavior;
- the exact set of control-rule variables, operators, and statuses;
- valve categories whose definitions may vary across simulation engines, especially generic-purpose or position-control variants;
- water-quality reaction options whose supported formulas and coefficient meanings may differ by solver.

These concepts have not been deleted because they currently carry real AOWIS functionality. Their backend-specific syntax and diagnostics are isolated, and the remaining semantic contract can now be reviewed independently.
