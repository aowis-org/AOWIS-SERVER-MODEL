# Map annotations — canonical MODEL contract (A1)

**Status:** A2 MODEL data and merge implementation complete (native Qt/C++
verification required after this boundary correction). Independent annotation
types, canonical codecs, strict validation, schema manifest and field-aware
merges are implemented. Geodesic calculations and annotation placement actions
are intentionally **outside MODEL**; a common geospatial service is required
before DVMDB integration.
DVMDB persistence integration remains phase B.
The legacy `NetworkHydraulicDistribution::map_labels` and its codec remain
until A3, so existing MODEL consumers are not broken in the middle of A2.
The corresponding native-storage design is in DVMDB's
`docs/map-annotation-persistence-design.md`.

## Purpose and boundaries

A **map annotation** is user-created, revisioned **project presentation data**,
not a hydraulic or drainage solver entity. A deliberately placed annotation is
not an automatically generated node ID, pipe ID, value label or map backdrop.
Annotations are editable in the relevant domain's Map Editor, rendered
read-only in its Map Monitor and selected through the existing Inspector.

MODEL owns the typed, domain-neutral annotation values, canonical encoding,
validation, field-aware merge and logical edit images. It does not implement
WGS84 geodesics, coordinate transformations, or annotation placement commands. DVMDB owns which project
and revision contains the layer, the record graph, working-copy journal,
revision history, authoritative membership and durability. GUI/Render own map
placement, screen rendering, hit testing, selection and transient previews.
No renderer-only state is authoritative. Do not add a second annotation database
or a separate annotation Commit.

## Proposed types and fields

The A2 implementation adds types under `include/aowis/model/map/`, not under
`hydraulic/`. The following types are implemented in MODEL; names here are canonical.

| Type | Canonical field | Meaning / constraints |
|---|---|---|
| `MapAnnotation` | `uuid: QUuid` | Non-null, immutable object identity; UUIDv7 on new creation |
| | `text: QString` | Plain Unicode text; not HTML/Markdown; non-blank after trimming; at most 4096 UTF-16 code units |
| | `position_wgs84: CoordinateWGS84` | Geographic world position when free-standing; last explicit position while anchored; valid finite longitude/latitude, optional finite GPS altitude |
| | `anchor: optional<MapAnnotationAnchor>` | Absent = free-standing; present = follow an in-scope spatial entity |
| | `style: MapAnnotationStyle` | Small, versioned, portable presentation settings (see below) |
| `MapAnnotationAnchor` | `entity_uuid: QUuid` | Non-null UUID of the attached entity |
| | `entity_type: QString` | Stable canonical MODEL entity type key; never inferred from UUID or UI text |
| | `offset_east_m: double` | Finite signed horizontal offset from the anchor point, in metres |
| | `offset_north_m: double` | Finite signed vertical-on-map (northing) offset from the anchor point, in metres |
| `MapAnnotationStyle` | `text_size: MapAnnotationTextSize` | `Small`, `Normal`, `Large`; defaults to `Normal` |
| | `show_background: bool` | Optional contrast panel; defaults to false |
| | `visible: bool` | Per-annotation visibility; defaults to true |
| `MapAnnotationLayer` | `uuid: QUuid` | Stable UUIDv7 for the independently owned layer |
| | `domain: MapAnnotationDomain` | Initial accepted values `Distribution`, `Drainage`; new domains extend this explicit enum |
| | `owner_network_uuid: optional<QUuid>` | Network ownership when a domain has a persisted engineering network |

`MapAnnotationLayer` is **metadata**, not an inline list of annotations.
An independently materialized layer snapshot is `(layer metadata, ordered
annotation members)`. The storage adapter owns native membership chunks;
MODEL must not include their UUIDs in canonical payloads. In-memory aggregate
helpers may expose members but must not create a second serialized source of
truth. For each layer, member order is revisioned and deterministic.

No free-form annotation ID/title separate from text is required in v1: the
stable UUID is identity, while lists show a text excerpt. No rich text, images,
shapes, arrows, font files, external links, or attachment blobs are in scope.
Styles are explicit and limited so that rendering remains portable in desktop
and WebAssembly. Units in stored offsets are always canonical metres.

## Domain and ownership

1. An annotation belongs to **exactly one** `MapAnnotationLayer` in exactly
   one DVMDB project's revision snapshot. A member UUID cannot simultaneously
   appear in other layers or hydraulic collections in that project snapshot.
2. Distribution: exactly one layer per `NetworkHydraulicDistribution` UUID.
   `domain = Distribution` and `owner_network_uuid` **must** identify that
   network's persisted root in the *same project revision*. Distinct networks
   get distinct layers. The owning network's removal removes its layer and
   annotations atomically; annotations do not silently become project-global.
3. Drainage before a persisted drainage network exists: allow at most **one**
   project-domain layer (`domain = Drainage`, `owner_network_uuid = null`) per
   project snapshot. This is a real revisioned layer, **not** an invented
   hydraulic network or unversioned GUI cache. No Drainage annotation may be
   stored in the Distribution layer. When a drainage network becomes available,
   assigning/repartitioning the existing domain layer into network-owned
   layers is an explicit versioned operation, not an implicit schema fallback.
4. A layer cannot change domain. Re-parenting/repartitioning changes ownership
   only through validated, atomic MODEL edits and native membership changes;
   no silent cross-domain moves.
5. A project may contain several distribution networks and future drainage
   networks. The currently selected domain/view is not a persistence key.

The project UUID is implicit in DVMDB's **per-project repository**; neither
MODEL annotation nor layer needs a redundant project UUID field. Revision
UUIDs and branch IDs are also not embedded in annotation values: identical
model records can be shared by immutable snapshots and branches.

## Anchoring, positioning and lifecycle

**Initial supported attached targets:** point-position distribution assets
(`HydraulicNodeJunction`, `HydraulicNodeReservoir`, `HydraulicNodeTank`,
`HydraulicDemandPoint`) within the layer's owning network. The target registry
can grow to Drainage and other position-resolvable types without changing the
anchor data shape. Pipes/pumps/valves have line geometry rather than an
unambiguous attachment point; do not pretend that they already have a stable
anchor resolver. Reject unsupported target types at validation, rather than
rendering approximate or inconsistent attachments.

- A free annotation renders at `position_wgs84`.
- An attached annotation renders at the target's current WGS84 position plus
  its signed east/north metre offsets. For attached annotations,
  `position_wgs84` is a **last explicit/free position**, not a second
  authoritative computed position. It must never be continuously rewritten
  merely because the target moves. Position resolution uses an AOWIS-wide
  geospatial service: a WGS84 geodesic direct operation for an initial
  east/north metre offset and the corresponding inverse operation when
  attaching or dragging. It is not a UTM projection or a DEM/hydraulic
  elevation calculation. Use a robust maintained implementation (GeographicLib
  already exists in the GUI toolchain); do not embed a second Vincenty
  implementation in MODEL or introduce a dependency from DVMDB on the GUI or
  renderer. Both GUI/Render and server-side atomic edits must call the same
  independently buildable geographic core.
- Dragging an attached annotation updates its offsets in **one** logical edit;
  dragging a free annotation updates `position_wgs84` in one logical edit.
  Attaching computes the offset that preserves its displayed position;
  explicitly detaching sets `position_wgs84` to the current resolved position.
- If an anchor moves, the annotation follows in the same materialized
  revision; no extra annotation edit or duplicated coordinate state is needed.
- If an anchored target is deleted, the *same atomic logical transaction*
  detaches every referencing annotation, resolves and stores its last displayed
  position using the **pre-deletion** target geometry, then deletes the target.
  Deleting the target without this cleanup must fail referential validation.
- Deleting an annotation never modifies its target. Deleting a whole network
  atomically removes its annotation layer and members.
- No cross-domain, cross-network or dangling anchors. When no target types
  exist yet in Drainage, its annotations remain free-standing.
- WGS84 optional altitude is **GPS/geographic**, not hydraulic elevation;
  no datum conversion or DEM height is silently persisted. Renderer terrain
  placement is presentation logic unless the user explicitly enters altitude.

A newly clicked empty preview is not a persisted annotation. The first durable
creation requires valid non-blank text; inspector edits thereafter autosave
through the same fenced MODEL/DVMDB journal as all other network edits.

## Validation and merge contract

MODEL must validate (before publishing any in-memory mutation or DVMDB write):
non-null identity, supported domain/type/enum, finite coordinates and offsets,
valid geographic ranges, non-blank bounded text, and canonical serialization.
DVMDB must additionally check layer ownership, target kind, same-network
membership, outgoing reference index, uniqueness and correct deletion cleanup
in the complete candidate snapshot. A native record's UUID and type cannot
change in an update. Failed edits leave candidate, journal and HEAD unchanged.

Canonical `mergeModelFields` for kind `map.annotation` handles independent
annotation field changes separately:
`text`, `position_wgs84`, `anchor` (as one semantic unit), and each `style`
field. The annotation's UUID and layer domain/identity are immutable. Different
edits to the same field conflict; deletion vs modification conflicts;
independent inserts may combine deterministically. Concurrent conflicting
layer membership/ownership operations must not silently choose a winner.
`map.annotation_layer` merges `owner_network_uuid` only; UUID and domain
are immutable. Changing domain fails closed even if field choices are given.
After a merge, re-run complete cross-layer referential and ownership validation.
If a target moved on one branch while its anchored annotation text changed on
another, both changes can combine: its position is derived from the merged
target snapshot.

## Logical edits and history

An annotation create/update/delete is an ordinary canonical `ModelEditTransaction`
entity image (`MapAnnotation`). Layer metadata creation or re-parenting uses an
explicit typed root image (`MapAnnotationLayerRoot`) plus server-owned
membership intents, in the **same** fenced Save operation where necessary.
The shared annotation-edit service must treat placement/anchoring as an
all-or-nothing candidate edit; geometry operations never commit on their own; the caller publishes a before/after image through
`ModelEditTransactionCodec`. The MODEL transaction frame does not encode DVMDB chunk bytes. The existing
before/after SHA-256, retry-ID and Undo/Redo rules apply; a drag is one event,
not one event per intermediate mouse movement. A mixed operation (e.g. delete
junction + detach attached notes) must be indivisible at event and native
candidate boundaries.

Read-only revisions and monitors never emit mutations. Whole history,
reconstruction, merging and branch transfer must preserve annotations exactly
at each revision, including unborn HEAD candidates and cross-Commit history.
Do not declare a DVMDB integration complete until its hydraulic-only history
assumptions have been generalized or safely extended for annotation records.

## Clean break and implementation gates

- A2: add MODEL types, canonical codecs, validators and merge/transaction
  tests; do not add geographic mathematics to MODEL.
- A2 geospatial integration gate (before B3): use the existing GeographicLib
  and RenderGeo groundwork to establish an independently reusable AOWIS geo
  foundation for GUI/Render and DVMDB. Move annotation placement/anchor
  transformation and its geodesic tests to the appropriate shared service.
  The native MODEL build must not link a renderer to perform geo calculations.
- A3: delete `HydraulicMapLabel`, `NetworkHydraulicDistribution::map_labels`,
  old reference/codec/merge/field-manifest/history support and their obsolete
  tests. Hydraulic map backdrops and generated labels are **not** annotations
  and are unaffected.
- B1–B4: introduce the separate native layer, adapt draft/HTTP/history/merge,
  test immutable snapshots, deletion and reference closure, fail-closed unknown
  record kinds, crash/power loss and deterministic replay.
- C/D: Inspector and map interaction, shared Render, read-only Monitor,
  drainage scoped layer. No duplicated per-domain annotation UI code.

No migration reader or compatibility shim for unpublished `HydraulicMapLabel`
records is required. Reject obsolete formats explicitly; developers must
recreate incompatible test repositories. **A1 changed documentation only; A2 changes MODEL code and tests.**

## Geographic separation correction (after A2)

The initial A2 prototype incorrectly placed a new Vincenty direct/inverse
implementation and annotation placement mutators into MODEL. These were
removed before DVMDB integration. GeographicLib and `AOWIS::RenderGeo` already
exist in the GUI architecture, but coupling server persistence to a renderer
would be equally incorrect. Shared geodesy should be a small, independently
buildable core available to every AOWIS component (desktop, mobile, WASM,
DVMDB, Map) without bgfx, Qt Widgets, or map-view dependencies. The existing
MODEL `CoordinateWGS84` struct remains the persisted data contract, not the
source of coordinate mathematics.

Until the shared geodesy service is integrated, attaching/detaching annotations
is a **planned capability, not implemented MODEL functionality**. The
canonical annotation codec, validation, field merge, and edit frames are
already implemented and covered by MODEL tests.
