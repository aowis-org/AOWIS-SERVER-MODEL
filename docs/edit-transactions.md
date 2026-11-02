# Model edit transactions — offline-first foundation

A logical editor action is one **atomic**, **reversible** transaction containing
one or more typed entity replacements. An editing transaction is **not** a
revision-control Commit. A model is materialized by applying its edits in order.

## Canonical transaction envelope (`AOWE`, version 1)

The MODEL library owns this wire format. All integers are unsigned big-endian;
UUIDs are 16 RFC 4122 bytes. The transaction UUID is a UUIDv7; entity UUIDs
are model keys and may be any non-null UUID (EPANET imports use deterministic
UUIDv5 keys; editor-created entities use UUIDv7). Strings are UTF-8, length-prefixed
with a 16-bit byte count. Every transaction is encoded deterministically:

```
"AOWE"             4 bytes
version            u8 (1)
payload length     u32 (<= 8 MiB)
payload:
  transaction UUID 16 bytes (UUIDv7, stable retry key)
  operation name   u16 length + UTF-8 (max 128 bytes; ASCII identifier)
  description      u16 length + UTF-8 (max 512 bytes; display only)
  edit count       u16 (1..4096)
  edits, canonically sorted ascending by entity UUID bytes, unique:
    entity UUID     16 bytes (non-null; any version)
    entity type     u16 length + UTF-8 (max 128 bytes; ASCII identifier)
    before image    u32 length + bytes, or 0xffffffff for absent
    after image     u32 length + bytes, or 0xffffffff for absent
SHA-256(payload)   32 bytes
```

Images are the **canonical typed MODEL entity encoding**, not JSON, field
strings, or DVMDB envelopes. Each present image contains 1..1 MiB of data.
Absent `before` creates an entity; absent `after` deletes it. Both cannot be
absent, and identical before/after bytes are not an edit. Strict parsing rejects
truncation, trailing bytes, invalid UTF-8, invalid version, unsupported sizes,
unsorted/duplicate UUIDs, no-op edits, and digest mismatches.

The digest detects accidental corruption and enforces byte-exact payload
identity; it is **not** a cryptographic authentication or permission mechanism.
The per-entity codec must validate embedded entity identifiers and types before
applying the event. The transaction codec intentionally does not decode model
fields; it remains usable by all model domains.

### Hydraulic network-root metadata edits

`NetworkHydraulicRoot` is the special edit type for a network's root metadata
(title lines, timing, hydraulic/reaction/energy/report options, multi-species
settings and backdrop). Its edit UUID is **the network UUID**. Each image is a
canonical `ModelCodec<NetworkHydraulic>` payload with **all 18 entity lists
empty**; lists are not moved, duplicated or replaced by root edits.

Only before+after root updates are permitted. The network UUID cannot
change; its human-readable `id` may be renamed (subject to validation). The server verifies the before-image against its current native
manifest, validates the resulting complete network and creates the native
manifest record **itself** under the ordinary working-copy lease/generation
fence. The GUI sends a single root-only event with an empty `changes` array,
never native manifest bytes or membership intents. DVMDB appends the event and
persists the manifest as one atomic candidate update, retaining all native
membership chunks. A replay uses the logical-request fingerprint and journal
identity for confirmation. The event does not create a revision; Commit does.

The GUI captures root changes using the same durable event queue as entity
edits. Editing controls must use `HydraulicData::setNetworkRootMetadata` (or a
specialized setter such as `setSimulationHeadlossFormula`); direct mutation of
the in-memory root without capturing an event is not an accepted private edit.

### Poor connectivity

- The GUI applies an operation to its in-memory model immediately and queues
  the encoded transaction; rendering never waits for HTTP.
- Personal DVMDB durably appends events and materializes their entity or
  network-root images **in one atomic generation-checked write**. The MODEL
  codec itself remains serialization only, not a durable autosave engine.
- Every event retains its transaction UUID across retries. The DVMDB writer
  must deduplicate by `(working-copy UUID, transaction UUID)` and compare the
  entire content digest; same UUID with different content is a conflict.
- A lost HTTP acknowledgement is resolved by reading the authoritative durable
  generation / transaction status. Never guess success or replay blindly.
- On connection loss, retain pending events in an fsynced local queue, preserve
  operation order, and expose the difference between *edited* and *saved*.
  The private repository is local by preference; remote private repositories
  must have the same semantics.
- Each drag or multi-object edit yields **one logical event**. Intermediate
  mouse moves are preview state and never individual transactions.
- Don't ZIP each transaction. Small event frames are already compact. Batch
  multiple events for transfer; use optional zstd compression **only when a
  measured size reduction justifies it**, with a strict decompressed-size cap.
- Preserve undo/redo history cursor in DVMDB atomically with the materialized
  candidate. An undo/redo UI and journal are follow-up work, not yet provided
  by this initial codec.

No backward-compatibility readers are required for pre-release experimental
formats. Version 1 here describes the **current** format and is deliberately
rejected when mismatched; don't introduce legacy fallbacks.

## Ownership

`AOWIS-SERVER-MODEL` owns this transaction representation and the canonical
entity codec. DVMDB owns durable journals, revisions, fencing, generations and
publication. GUI owns operation boundaries, live previews, queue and UI state.
