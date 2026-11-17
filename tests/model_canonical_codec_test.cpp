#include <aowis/model/serialization/model_codec.h>
#include <aowis/model/serialization/model_validation.h>
#include <aowis/model/serialization/model_field_merge.h>
#include <aowis/model/edit/hydraulic_distribution_root_edit.h>

#include <QUuid>
#include <QByteArray>
#include <QMap>
#include <QString>
#include <QStringList>
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
    NetworkHydraulicDistribution network;
    network.uuid = QUuid::createUuid();
    network.nodes_junctions.append(junction);
    const QByteArray encodedNetwork = aowis::model::ModelCodec<NetworkHydraulicDistribution>::encode(network);
    const std::optional<NetworkHydraulicDistribution> decodedNetwork =
        aowis::model::ModelCodec<NetworkHydraulicDistribution>::decode(encodedNetwork);
    // A3 deliberately rejects the previous pre-release canonical envelope.
    QByteArray obsoleteFormat = encodedNetwork;
    if (obsoleteFormat.size() < 8)
        return EXIT_FAILURE;
    obsoleteFormat[7] = '\x01'; // big-endian codec format 1, rather than 2
    if (aowis::model::ModelCodec<NetworkHydraulicDistribution>::decode(obsoleteFormat))
        return EXIT_FAILURE;
    if (encodedNetwork.isEmpty() || !decodedNetwork ||
        decodedNetwork->nodes_junctions.size() != 1 ||
        decodedNetwork->nodes_junctions.first().uuid != junction.uuid)
        return EXIT_FAILURE;
    if (!aowis::model::validateHydraulicDistributionNetwork(network).valid())
        return EXIT_FAILURE;
    if (aowis::model::mergeModelFields(junction.uuid, QStringLiteral("not-a-kind"), bytes, bytes, bytes))
        return EXIT_FAILURE;

    // Project units: optional root metadata that round-trips canonically and
    // is validated against the presentation-unit registry.
    NetworkHydraulicDistribution withUnits = network;
    NetworkPresentationUnits units;
    units.name = QStringLiteral("Utility SI");
    units.derived_from_profile = QStringLiteral("LPS");
    units.units.append(NetworkPresentationUnit{QStringLiteral("pressure_head"), QStringLiteral("bar")});
    units.units.append(NetworkPresentationUnit{QStringLiteral("volumetric_flow_rate"), QStringLiteral("L/s")});
    withUnits.presentation_units = units;
    const QByteArray encodedUnits = aowis::model::ModelCodec<NetworkHydraulicDistribution>::encode(withUnits);
    const std::optional<NetworkHydraulicDistribution> decodedUnits =
        aowis::model::ModelCodec<NetworkHydraulicDistribution>::decode(encodedUnits);
    if (encodedUnits.isEmpty() || encodedUnits == encodedNetwork || !decodedUnits ||
        !decodedUnits->presentation_units ||
        decodedUnits->presentation_units->name != units.name ||
        decodedUnits->presentation_units->derived_from_profile != units.derived_from_profile ||
        decodedUnits->presentation_units->units.size() != 2 ||
        decodedUnits->presentation_units->units.at(1).unit != QStringLiteral("L/s") ||
        aowis::model::ModelCodec<NetworkHydraulicDistribution>::encode(*decodedUnits) != encodedUnits)
        return EXIT_FAILURE;
    if (decodedNetwork->presentation_units.has_value())
        return EXIT_FAILURE;
    if (!aowis::model::validateHydraulicDistributionNetwork(withUnits).valid())
        return EXIT_FAILURE;
    NetworkHydraulicDistribution badUnit = withUnits;
    badUnit.presentation_units->units[1].unit = QStringLiteral("furlong/fortnight");
    NetworkHydraulicDistribution unsorted = withUnits;
    std::swap(unsorted.presentation_units->units[0], unsorted.presentation_units->units[1]);
    NetworkHydraulicDistribution unknown = withUnits;
    unknown.presentation_units->units.append(NetworkPresentationUnit{QStringLiteral("zz_not_a_quantity"), QStringLiteral("m")});
    NetworkHydraulicDistribution unnamed = withUnits;
    unnamed.presentation_units->name = QStringLiteral("  ");
    if (aowis::model::validateHydraulicDistributionNetwork(badUnit).valid() ||
        aowis::model::validateHydraulicDistributionNetwork(unsorted).valid() ||
        aowis::model::validateHydraulicDistributionNetwork(unknown).valid() ||
        aowis::model::validateHydraulicDistributionNetwork(unnamed).valid())
        return EXIT_FAILURE;

    // Network-root metadata merges by field, project units by quantity.
    {
        using aowis::model::hydraulicDistributionRootImage;
        const QString rootKind = QStringLiteral("hydraulic.network_root");
        NetworkHydraulicDistribution base = withUnits;
        NetworkHydraulicDistribution ours = base;
        NetworkHydraulicDistribution theirs = base;
        ours.title_line_1 = QStringLiteral("Ours");
        ours.presentation_units->units[1].unit = QStringLiteral("m3/h");      // flow
        theirs.options_hydraulic.accuracy = 0.0005;
        theirs.presentation_units->units[0].unit = QStringLiteral("psi");     // pressure head
        theirs.presentation_units->name = QStringLiteral("Mixed");
        const std::optional<aowis::model::ModelFieldMerge> combined = aowis::model::mergeModelFields(
            base.uuid, rootKind, hydraulicDistributionRootImage(base), hydraulicDistributionRootImage(ours), hydraulicDistributionRootImage(theirs));
        if (!combined || !combined->conflicting_fields.isEmpty() || combined->content.isEmpty())
            return EXIT_FAILURE;
        const std::optional<NetworkHydraulicDistribution> mergedRoot =
            aowis::model::decodeHydraulicDistributionRootImage(combined->content);
        if (!mergedRoot || mergedRoot->title_line_1 != QStringLiteral("Ours") ||
            mergedRoot->options_hydraulic.accuracy != 0.0005 ||
            !mergedRoot->presentation_units ||
            mergedRoot->presentation_units->name != QStringLiteral("Mixed") ||
            mergedRoot->presentation_units->units.size() != 2 ||
            mergedRoot->presentation_units->units.at(0).unit != QStringLiteral("psi") ||
            mergedRoot->presentation_units->units.at(1).unit != QStringLiteral("m3/h"))
            return EXIT_FAILURE;

        // Same quantity changed differently: a conflict naming that quantity,
        // resolvable by an explicit choice.
        NetworkHydraulicDistribution rival = base;
        rival.presentation_units->units[1].unit = QStringLiteral("ML/d");
        const QString flowField = QStringLiteral("presentation_units.units.volumetric_flow_rate");
        const std::optional<aowis::model::ModelFieldMerge> clash = aowis::model::mergeModelFields(
            base.uuid, rootKind, hydraulicDistributionRootImage(base), hydraulicDistributionRootImage(ours), hydraulicDistributionRootImage(rival));
        if (!clash || clash->conflicting_fields != QStringList{flowField} || !clash->content.isEmpty())
            return EXIT_FAILURE;
        const std::optional<aowis::model::ModelFieldMerge> chosen = aowis::model::mergeModelFields(
            base.uuid, rootKind, hydraulicDistributionRootImage(base), hydraulicDistributionRootImage(ours), hydraulicDistributionRootImage(rival),
            QMap<QString, bool>{{flowField, true}});
        const std::optional<NetworkHydraulicDistribution> chosenRoot = chosen
            ? aowis::model::decodeHydraulicDistributionRootImage(chosen->content) : std::nullopt;
        if (!chosenRoot || !chosenRoot->presentation_units ||
            chosenRoot->presentation_units->units.at(1).unit != QStringLiteral("ML/d") ||
            chosenRoot->title_line_1 != QStringLiteral("Ours"))
            return EXIT_FAILURE;

        // Removing the unit set while the other branch changed it is one decision.
        NetworkHydraulicDistribution removed = base;
        removed.presentation_units.reset();
        const std::optional<aowis::model::ModelFieldMerge> removal = aowis::model::mergeModelFields(
            base.uuid, rootKind, hydraulicDistributionRootImage(base), hydraulicDistributionRootImage(ours), hydraulicDistributionRootImage(removed));
        if (!removal || removal->conflicting_fields != QStringList{QStringLiteral("presentation_units")})
            return EXIT_FAILURE;

        // Both branches add units independently: quantities combine.
        NetworkHydraulicDistribution plain = base;
        plain.presentation_units.reset();
        NetworkHydraulicDistribution addOurs = plain;
        NetworkHydraulicDistribution addTheirs = plain;
        NetworkPresentationUnits flowOnly;
        flowOnly.name = QStringLiteral("Site");
        flowOnly.units.append(NetworkPresentationUnit{QStringLiteral("volumetric_flow_rate"), QStringLiteral("L/s")});
        NetworkPresentationUnits pressureOnly = flowOnly;
        pressureOnly.units = {NetworkPresentationUnit{QStringLiteral("pressure_head"), QStringLiteral("bar")}};
        addOurs.presentation_units = flowOnly;
        addTheirs.presentation_units = pressureOnly;
        const std::optional<aowis::model::ModelFieldMerge> added = aowis::model::mergeModelFields(
            base.uuid, rootKind, hydraulicDistributionRootImage(plain), hydraulicDistributionRootImage(addOurs), hydraulicDistributionRootImage(addTheirs));
        const std::optional<NetworkHydraulicDistribution> addedRoot = added && added->conflicting_fields.isEmpty()
            ? aowis::model::decodeHydraulicDistributionRootImage(added->content) : std::nullopt;
        if (!addedRoot || !addedRoot->presentation_units ||
            addedRoot->presentation_units->units.size() != 2 ||
            addedRoot->presentation_units->units.at(0).quantity != QStringLiteral("pressure_head") ||
            !aowis::model::validateHydraulicDistributionNetwork(*addedRoot).valid())
            return EXIT_FAILURE;

        // A non-root image (with collections) is never field-merged.
        if (aowis::model::mergeModelFields(base.uuid, rootKind, encodedUnits, encodedUnits, encodedUnits))
            return EXIT_FAILURE;
    }

    std::cout << "AOWIS MODEL canonical codec checks passed\n";
    return EXIT_SUCCESS;
}
