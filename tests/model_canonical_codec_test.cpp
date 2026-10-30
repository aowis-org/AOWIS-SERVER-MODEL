#include <aowis/model/serialization/model_codec.h>
#include <aowis/model/serialization/model_validation.h>
#include <aowis/model/serialization/model_field_merge.h>

#include <QUuid>
#include <QByteArray>
#include <QString>
#include <cstdlib>
#include <iostream>
#include <optional>

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
    std::cout << "AOWIS MODEL canonical codec checks passed\n";
    return EXIT_SUCCESS;
}
