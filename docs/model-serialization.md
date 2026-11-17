# Canonical model serialization (current pre-release format)

AOWIS-SERVER-MODEL **owns** the one typed `aowis::model::ModelCodec<T>` and
hydraulic distribution network validation. Both GUI and DVMDB link `AOWIS::Model`; neither
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
historical fixtures. The `NetworkHydraulicDistribution` codec uses its own root type identifier;
old `NetworkHydraulic` root payloads are not accepted. This changes the root
payload bytes and must be coordinated with DVMDB and other consumers; the
individual hydraulic entity codecs and their field layouts are unchanged.

Configure-time `tools/check_model_codec.py` verifies fields and numeric rules.
`docs/model_codec_fields.json` is the authoritative field manifest; the
`model_enum_values.inc` table is generated for the current Model definitions.
