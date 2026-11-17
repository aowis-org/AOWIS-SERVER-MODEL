#include <aowis/model/serialization/model_codec.h>
#include <aowis/model/serialization/model_validation.h>
#include <aowis/model/serialization/model_field_merge.h>

#include <QUuid>
#include <QDataStream>
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
    NetworkHydraulicDistribution network;
    network.uuid = QUuid::createUuid();
    network.nodes_junctions.append(junction);
    const QByteArray encodedNetwork = aowis::model::ModelCodec<NetworkHydraulicDistribution>::encode(network);
    const std::optional<NetworkHydraulicDistribution> decodedNetwork =
        aowis::model::ModelCodec<NetworkHydraulicDistribution>::decode(encodedNetwork);
    if (encodedNetwork.isEmpty() || !decodedNetwork ||
        decodedNetwork->nodes_junctions.size() != 1 ||
        decodedNetwork->nodes_junctions.first().uuid != junction.uuid)
        return EXIT_FAILURE;
    // The root model's persisted type identifier must match its domain-specific
    // C++ name; the old generic hydraulic identifier is no longer accepted.
    QDataStream header(encodedNetwork);
    header.setVersion(QDataStream::Qt_6_0);
    quint32 magic = 0;
    quint32 format = 0;
    QString typeName;
    header >> magic >> format >> typeName;
    if (header.status() != QDataStream::Ok ||
        typeName != QStringLiteral("NetworkHydraulicDistribution"))
        return EXIT_FAILURE;
    if (!aowis::model::validateHydraulicDistributionNetwork(network).valid())
        return EXIT_FAILURE;
    if (aowis::model::mergeModelFields(junction.uuid, QStringLiteral("not-a-kind"), bytes, bytes, bytes))
        return EXIT_FAILURE;
    std::cout << "AOWIS MODEL canonical codec checks passed\n";
    return EXIT_SUCCESS;
}
