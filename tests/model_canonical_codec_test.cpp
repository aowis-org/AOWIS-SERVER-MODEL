#include <aowis/model/serialization/model_codec.h>
#include <aowis/model/serialization/model_validation.h>
#include <aowis/model/serialization/model_field_merge.h>

#include <QUuid>
#include <QByteArray>
#include <QString>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <utility>

int main()
{
    HydraulicNodeJunction junction;
    junction.uuid = QUuid::createUuid();
    junction.elevation_m = 24.5;
    const QByteArray bytes = aowis::model::ModelCodec<HydraulicNodeJunction>::encode(junction);
    if (bytes.isEmpty() || bytes != aowis::model::ModelCodec<HydraulicNodeJunction>::encode(junction))
        return EXIT_FAILURE;
    const std::optional<HydraulicNodeJunction> roundTrip =
        aowis::model::ModelCodec<HydraulicNodeJunction>::decode(bytes);
    if (!roundTrip || roundTrip->uuid != junction.uuid || roundTrip->elevation_m != 24.5)
        return EXIT_FAILURE;
    if (aowis::model::ModelCodec<HydraulicNodeJunction>::decode(bytes.left(bytes.size() - 1)))
        return EXIT_FAILURE;
    NetworkHydraulic network;
    network.uuid = QUuid::createUuid();
    network.nodes_junctions.append(junction);
    const QByteArray encodedNetwork = aowis::model::ModelCodec<NetworkHydraulic>::encode(network);
    const std::optional<NetworkHydraulic> decodedNetwork =
        aowis::model::ModelCodec<NetworkHydraulic>::decode(encodedNetwork);
    if (encodedNetwork.isEmpty() || !decodedNetwork ||
        decodedNetwork->nodes_junctions.size() != 1 ||
        decodedNetwork->nodes_junctions.first().uuid != junction.uuid)
        return EXIT_FAILURE;
    if (!aowis::model::validateHydraulicNetwork(network).valid())
        return EXIT_FAILURE;
    if (aowis::model::mergeModelFields(junction.uuid, QStringLiteral("not-a-kind"), bytes, bytes, bytes))
        return EXIT_FAILURE;

    // Project units: optional root metadata that round-trips canonically and
    // is validated against the presentation-unit registry.
    NetworkHydraulic withUnits = network;
    NetworkPresentationUnits units;
    units.name = QStringLiteral("Utility SI");
    units.derived_from_profile = QStringLiteral("LPS");
    units.units.append(NetworkPresentationUnit{QStringLiteral("pressure_head"), QStringLiteral("bar")});
    units.units.append(NetworkPresentationUnit{QStringLiteral("volumetric_flow_rate"), QStringLiteral("L/s")});
    withUnits.presentation_units = units;
    const QByteArray encodedUnits = aowis::model::ModelCodec<NetworkHydraulic>::encode(withUnits);
    const std::optional<NetworkHydraulic> decodedUnits =
        aowis::model::ModelCodec<NetworkHydraulic>::decode(encodedUnits);
    if (encodedUnits.isEmpty() || encodedUnits == encodedNetwork || !decodedUnits ||
        !decodedUnits->presentation_units ||
        decodedUnits->presentation_units->name != units.name ||
        decodedUnits->presentation_units->derived_from_profile != units.derived_from_profile ||
        decodedUnits->presentation_units->units.size() != 2 ||
        decodedUnits->presentation_units->units.at(1).unit != QStringLiteral("L/s") ||
        aowis::model::ModelCodec<NetworkHydraulic>::encode(*decodedUnits) != encodedUnits)
        return EXIT_FAILURE;
    if (decodedNetwork->presentation_units.has_value())
        return EXIT_FAILURE;
    if (!aowis::model::validateHydraulicNetwork(withUnits).valid())
        return EXIT_FAILURE;
    NetworkHydraulic badUnit = withUnits;
    badUnit.presentation_units->units[1].unit = QStringLiteral("furlong/fortnight");
    NetworkHydraulic unsorted = withUnits;
    std::swap(unsorted.presentation_units->units[0], unsorted.presentation_units->units[1]);
    NetworkHydraulic unknown = withUnits;
    unknown.presentation_units->units.append(NetworkPresentationUnit{QStringLiteral("zz_not_a_quantity"), QStringLiteral("m")});
    NetworkHydraulic unnamed = withUnits;
    unnamed.presentation_units->name = QStringLiteral("  ");
    if (aowis::model::validateHydraulicNetwork(badUnit).valid() ||
        aowis::model::validateHydraulicNetwork(unsorted).valid() ||
        aowis::model::validateHydraulicNetwork(unknown).valid() ||
        aowis::model::validateHydraulicNetwork(unnamed).valid())
        return EXIT_FAILURE;
    std::cout << "AOWIS MODEL canonical codec checks passed\n";
    return EXIT_SUCCESS;
}
