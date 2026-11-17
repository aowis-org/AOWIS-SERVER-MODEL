#ifndef AOWIS_MODEL_HYDRAULIC_ROOT_EDIT_H
#define AOWIS_MODEL_HYDRAULIC_ROOT_EDIT_H

#include <aowis/model/hydraulic/network_hydraulic.h>
#include <aowis/model/serialization/model_codec.h>

#include <optional>
#include <utility>

// A network-root edit contains ONLY scalar/structured network-wide metadata.
// Collections are stored in separate DVMDB membership chunks, and may never
// travel as part of a root editing event. The canonical full-network codec is
// used only for the metadata projection; no alternative serialization exists.
namespace aowis::model {
inline constexpr const char *hydraulicRootEditType = "NetworkHydraulicRoot";

inline void clearHydraulicRootCollections(NetworkHydraulic &network)
{
    network.pipe_materials.clear();
    network.patterns_time.clear();
    network.curves_tank_volume.clear();
    network.curves_pump_head.clear();
    network.curves_pump_efficiency.clear();
    network.curves_valve_headloss.clear();
    network.curves_valve_characteristic.clear();
    network.curves_generic.clear();
    network.nodes_reservoirs.clear();
    network.nodes_junctions.clear();
    network.nodes_tanks.clear();
    network.demand_points.clear();
    network.links_pipes.clear();
    network.links_pumps.clear();
    network.links_valves.clear();
    network.controls_simple.clear();
    network.controls_rules.clear();
    network.map_labels.clear();
}

inline NetworkHydraulic hydraulicRootMetadata(const NetworkHydraulic &network)
{
    NetworkHydraulic metadata = network;
    clearHydraulicRootCollections(metadata);
    return metadata;
}

inline QByteArray hydraulicRootImage(const NetworkHydraulic &network)
{
    return ModelCodec<NetworkHydraulic>::encode(hydraulicRootMetadata(network));
}

inline std::optional<NetworkHydraulic> decodeHydraulicRootImage(
    const QByteArray &image, QString *error = nullptr)
{
    const std::optional<NetworkHydraulic> result =
        ModelCodec<NetworkHydraulic>::decode(image, error);
    if (!result.has_value() || result->uuid.isNull() ||
        hydraulicRootImage(*result) != image ||
        ModelCodec<NetworkHydraulic>::encode(*result) != image)
    {
        if (error) *error = QStringLiteral("Invalid or noncanonical network-root metadata image");
        return std::nullopt;
    }
    return result;
}

inline bool applyHydraulicRootMetadata(NetworkHydraulic &network,
                                       const NetworkHydraulic &metadata)
{
    if (metadata.uuid != network.uuid)
        return false;
    NetworkHydraulic updated = metadata;
    updated.pipe_materials = network.pipe_materials;
    updated.patterns_time = network.patterns_time;
    updated.curves_tank_volume = network.curves_tank_volume;
    updated.curves_pump_head = network.curves_pump_head;
    updated.curves_pump_efficiency = network.curves_pump_efficiency;
    updated.curves_valve_headloss = network.curves_valve_headloss;
    updated.curves_valve_characteristic = network.curves_valve_characteristic;
    updated.curves_generic = network.curves_generic;
    updated.nodes_reservoirs = network.nodes_reservoirs;
    updated.nodes_junctions = network.nodes_junctions;
    updated.nodes_tanks = network.nodes_tanks;
    updated.demand_points = network.demand_points;
    updated.links_pipes = network.links_pipes;
    updated.links_pumps = network.links_pumps;
    updated.links_valves = network.links_valves;
    updated.controls_simple = network.controls_simple;
    updated.controls_rules = network.controls_rules;
    updated.map_labels = network.map_labels;
    network = std::move(updated);
    return true;
}
}

#endif
