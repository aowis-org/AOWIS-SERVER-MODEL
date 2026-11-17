# Canonical model serialization (current pre-release format)

AOWIS-SERVER-MODEL **owns** the one typed `aowis::model::ModelCodec<T>` and
hydraulic model validation. Both GUI and DVMDB link `AOWIS::Model`; neither
owns a competing implementation. Model schema manifest, enum generation,
field-aware comparisons and numeric value rules live here. DVMDB only supplies
storage-specific record envelopes, revisions, working-copy state and HTTP.

The canonical entity payload uses deterministic binary `QDataStream` fields,
not ZIP-compressed JSON. Small logical editing transactions are kept as
uncompressed, bounded binary frames with integrity checks. Use compression on
larger transport batches only when benchmarks justify it. JSON is for human
inspection and API envelopes, not persisted model semantics.

AOWIS has **no production-format compatibility obligation before v1**. During
pre-release development we may change the magic, layout or version and reject
obsolete data rather than maintain old readers. Current tests validate current
schema, corruption checks, round-tripping and rejects for invalid values, not
historical fixtures. This migration deliberately does not alter current bytes;
that is a refactor risk-management choice, not a guarantee about future formats.

Configure-time `tools/check_model_codec.py` verifies fields and numeric rules.
`docs/model_codec_fields.json` is the authoritative field manifest; the
`model_enum_values.inc` table is generated for the current Model definitions.

## Project units

`NetworkHydraulicDistribution::presentation_units` (`NetworkPresentationUnits`, empty by
default) stores the network's presentation units: a name, the informational
id of the profile it was copied from, and a sorted list of
`{quantity, unit}` choices. It is root metadata, so it is versioned, committed and
transferred with the network root, and merged **per quantity** (see below). It is a self-contained copy, never a profile reference,
and it never alters a stored value: every measurement stays canonical.

## Network-root field merge

`mergeModelFields(uuid, "hydraulic.network_root", base, left, right, choices)`
merges three network-root metadata images (collections are never part of a
root image; their membership merges separately in DVMDB). Independent changes
combine; only a field changed differently on both branches conflicts, and an
explicit choice for exactly that field name resolves it.

| Part | Conflict granularity (field name) |
|---|---|
| Scalar root fields | each field (`title_line_1`, `duration_s`, `report_statistic`, …) |
| `options_hydraulic`, `options_reaction`, `options_energy`, `options_report`, `map_backdrop` | each field of the group (`options_hydraulic.accuracy`, `map_backdrop.file`, …) |
| `multi_species` | the whole group (its lists have no stable per-field identity) |
| Project units | `presentation_units.name`, `presentation_units.derived_from_profile`, each quantity `presentation_units.units.<quantity>`; adding/removing the whole set while the other branch changed it: `presentation_units` |

A quantity removed on one branch and unchanged on the other is removed;
quantities added on both branches combine. `tools/check_model_codec.py` fails
the build if a root field, or a field of a decomposed group, is missing from
`mergeNetworkRootFields()`. A merged root is still revalidated as a whole
network before DVMDB publishes it.
