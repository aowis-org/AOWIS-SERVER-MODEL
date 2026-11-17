#include <aowis/model/edit/model_edit_history.h>
#include <aowis/model/edit/hydraulic_root_edit.h>
#include <aowis/model/serialization/model_codec.h>

#include <QByteArray>
#include <QString>
#include <QUuid>

#include <cstdlib>
#include <iostream>
#include <optional>

using aowis::model::ModelEditHistory;
using aowis::model::ModelEditTransaction;
using aowis::model::ModelEditTransactionCodec;
using aowis::model::ModelEntityEdit;
using aowis::model::ModelHistoryImage;
using aowis::model::ModelHistoryImageMap;

namespace {
void require(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << "MODEL history regression failed: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

QUuid uuid(quint64 suffix)
{
    return QUuid(QStringLiteral("019a0234-5678-7a12-8a01-%1")
                     .arg(suffix, 12, 16, QLatin1Char('0')));
}

template<typename Entity>
void createEntity(ModelEditTransaction &action, const QUuid &identity,
                  const QString &typeName)
{
    Entity entity;
    entity.uuid = identity;
    entity.id = QStringLiteral("HIST-01-") + typeName;
    const QByteArray bytes = aowis::model::ModelCodec<Entity>::encode(entity);
    require(!bytes.isEmpty(), "canonical hydraulic entity encoding failed");
    action.edits.append(ModelEntityEdit{identity, typeName, std::nullopt, bytes});
}

ModelHistoryImageMap startingState(const QUuid &networkId, const QByteArray &rootImage,
                                   const QUuid &junctionId, const QByteArray &junctionImage,
                                   const QUuid &pipeId, const QByteArray &pipeImage)
{
    ModelHistoryImageMap state;
    state.insert(networkId, {QString::fromLatin1(aowis::model::hydraulicRootEditType), rootImage});
    state.insert(junctionId, {QStringLiteral("HydraulicNodeJunction"), junctionImage});
    state.insert(pipeId, {QStringLiteral("HydraulicLinkPipe"), pipeImage});
    // An untouched entity must survive every transition unchanged.
    state.insert(uuid(700), {QStringLiteral("HydraulicMapLabel"), QByteArray("untouched")});
    return state;
}
} // namespace

int main()
{
    NetworkHydraulic rootBefore;
    rootBefore.uuid = uuid(100);
    rootBefore.id = QStringLiteral("Distribution");
    rootBefore.title_line_1 = QStringLiteral("Before");
    rootBefore.duration_s = 3600;
    NetworkHydraulic rootAfter = rootBefore;
    rootAfter.title_line_1 = QStringLiteral("After");
    rootAfter.duration_s = 7200;
    rootAfter.map_backdrop.file = QStringLiteral("history.png");
    const QByteArray beforeRoot = aowis::model::hydraulicRootImage(rootBefore);
    const QByteArray afterRoot = aowis::model::hydraulicRootImage(rootAfter);
    require(!beforeRoot.isEmpty() && !afterRoot.isEmpty() && beforeRoot != afterRoot,
            "network root metadata images must be distinct canonical bytes");

    // Imported model keys can be UUIDv5; only history operation IDs must be v7.
    const QUuid importedJunction(QStringLiteral("6f1c2d3e-4b5a-5c6d-9e8f-0a1b2c3d4e5f"));
    HydraulicNodeJunction junctionBefore;
    junctionBefore.uuid = importedJunction;
    junctionBefore.id = QStringLiteral("Imported-J1");
    junctionBefore.elevation_m = 12;
    HydraulicNodeJunction junctionAfter = junctionBefore;
    junctionAfter.elevation_m = 18;
    const QByteArray oldJunction = aowis::model::ModelCodec<HydraulicNodeJunction>::encode(junctionBefore);
    const QByteArray newJunction = aowis::model::ModelCodec<HydraulicNodeJunction>::encode(junctionAfter);
    require(!oldJunction.isEmpty() && oldJunction != newJunction,
            "imported UUIDv5 junction update must retain canonical identity");

    HydraulicLinkPipe removedPipe;
    removedPipe.uuid = uuid(101);
    removedPipe.id = QStringLiteral("Removed-P1");
    const QByteArray oldPipe = aowis::model::ModelCodec<HydraulicLinkPipe>::encode(removedPipe);
    require(!oldPipe.isEmpty(), "canonical pipe deletion image is available");

    ModelEditTransaction action;
    action.transactionId = uuid(500);
    action.operation = QStringLiteral("hydraulic.network.bulk-edit");
    action.description = QStringLiteral("Mixed root, creates, update and delete");
    quint64 nextIdentity = 1;
#define ADD_HYDRAULIC_CREATE(type) \
    createEntity<type>(action, uuid(nextIdentity++), QStringLiteral(#type));
    ADD_HYDRAULIC_CREATE(HydraulicPipeMaterial)
    ADD_HYDRAULIC_CREATE(HydraulicPatternTime)
    ADD_HYDRAULIC_CREATE(HydraulicCurveTankVolume)
    ADD_HYDRAULIC_CREATE(HydraulicCurvePumpHead)
    ADD_HYDRAULIC_CREATE(HydraulicCurvePumpEfficiency)
    ADD_HYDRAULIC_CREATE(HydraulicCurveValveHeadloss)
    ADD_HYDRAULIC_CREATE(HydraulicCurveValveCharacteristic)
    ADD_HYDRAULIC_CREATE(HydraulicCurveGeneric)
    ADD_HYDRAULIC_CREATE(HydraulicNodeReservoir)
    ADD_HYDRAULIC_CREATE(HydraulicNodeJunction)
    ADD_HYDRAULIC_CREATE(HydraulicNodeTank)
    ADD_HYDRAULIC_CREATE(HydraulicDemandPoint)
    ADD_HYDRAULIC_CREATE(HydraulicLinkPipe)
    ADD_HYDRAULIC_CREATE(HydraulicLinkPump)
    ADD_HYDRAULIC_CREATE(HydraulicLinkValve)
    ADD_HYDRAULIC_CREATE(HydraulicControlSimple)
    ADD_HYDRAULIC_CREATE(HydraulicControlRule)
    ADD_HYDRAULIC_CREATE(HydraulicMapLabel)
#undef ADD_HYDRAULIC_CREATE
    require(nextIdentity == 19, "all 18 hydraulic entity collections participate");
    action.edits.append(ModelEntityEdit{rootBefore.uuid,
        QString::fromLatin1(aowis::model::hydraulicRootEditType), beforeRoot, afterRoot});
    action.edits.append(ModelEntityEdit{importedJunction,
        QStringLiteral("HydraulicNodeJunction"), oldJunction, newJunction});
    action.edits.append(ModelEntityEdit{removedPipe.uuid,
        QStringLiteral("HydraulicLinkPipe"), oldPipe, std::nullopt});
    require(action.edits.size() == 21, "one logical transaction groups 21 typed edits");

    QString error;
    const QByteArray originalBytes = ModelEditTransactionCodec::encode(action, &error);
    require(!originalBytes.isEmpty() && error.isEmpty(), "canonical mixed transaction encoding");
    const std::optional<ModelEditTransaction> decoded = ModelEditTransactionCodec::decode(originalBytes, &error);
    require(decoded.has_value() && error.isEmpty(), "canonical mixed transaction decoding");

    ModelHistoryImageMap state = startingState(rootBefore.uuid, beforeRoot,
                                               importedJunction, oldJunction,
                                               removedPipe.uuid, oldPipe);
    const ModelHistoryImageMap before = state;
    require(ModelEditHistory::apply(*decoded, state, &error) && error.isEmpty(),
            "mixed forward action should atomically apply");
    const ModelHistoryImageMap after = state;
    require(after.size() == before.size() + 18 - 1,
            "create and delete counts must match the resulting state");
    require(after.value(rootBefore.uuid).bytes == afterRoot &&
            after.value(importedJunction).bytes == newJunction &&
            !after.contains(removedPipe.uuid) &&
            after.value(uuid(700)).bytes == QByteArray("untouched"),
            "canonical mixed action changed only requested images");

    const std::optional<ModelEditTransaction> inverse =
        ModelEditHistory::inverse(*decoded, uuid(501), &error);
    require(inverse.has_value() && inverse->operation == QStringLiteral("history.undo") &&
            inverse->transactionId != decoded->transactionId && error.isEmpty(),
            "undo is a new well-formed durable event");
    const QByteArray inverseBytes = ModelEditTransactionCodec::encode(*inverse, &error);
    require(!inverseBytes.isEmpty() &&
            ModelEditTransactionCodec::decode(inverseBytes).has_value(),
            "inverse is transportable in the unchanged AOWE codec");
    require(ModelEditHistory::apply(*inverse, state, &error) && state == before,
            "all 18 creates, root metadata, update and delete undo byte-exactly");

    const std::optional<ModelEditTransaction> redo =
        ModelEditHistory::redo(*decoded, uuid(502), &error);
    require(redo.has_value() && redo->operation == QStringLiteral("history.redo") &&
            redo->transactionId != decoded->transactionId && redo->transactionId != inverse->transactionId &&
            ModelEditTransactionCodec::encode(*redo) != originalBytes,
            "redo uses fresh identity, not a replay of the original operation ID");
    require(ModelEditHistory::apply(*redo, state, &error) && state == after,
            "redo restores exact byte-for-byte post-action state");

    // A failed attempt to redo twice must not change any part of the state.
    const ModelHistoryImageMap stable = state;
    require(!ModelEditHistory::apply(*redo, state, &error) && !error.isEmpty() && state == stable,
            "replayed create with the same before-state is refused atomically");

    ModelHistoryImageMap stale = before;
    stale.insert(importedJunction, {QStringLiteral("HydraulicNodeJunction"), QByteArray("stale")});
    const ModelHistoryImageMap unchangedStale = stale;
    require(!ModelEditHistory::apply(*decoded, stale, &error) && state == stable &&
            stale == unchangedStale && error.contains(QStringLiteral("before-image")),
            "stale before-image on a late event never partially applies earlier edits");
    stale = before;
    stale.insert(importedJunction, {QStringLiteral("HydraulicNodeTank"), oldJunction});
    require(!ModelEditHistory::apply(*decoded, stale, &error),
            "wrong entity type cannot pass a byte-equal before-image");
    stale = before;
    stale.remove(removedPipe.uuid);
    require(!ModelEditHistory::apply(*decoded, stale, &error),
            "deleting an already absent entity is refused");

    require(!ModelEditHistory::inverse(*decoded, decoded->transactionId, &error).has_value(),
            "inverse cannot reuse the original action UUID");
    const QUuid nonV7(QStringLiteral("6f1c2d3e-4b5a-5c6d-9e8f-0a1b2c3d4e5f"));
    require(!ModelEditHistory::redo(*decoded, nonV7, &error).has_value(),
            "history operation ID must remain UUIDv7");
    ModelEditTransaction duplicate = *decoded;
    duplicate.edits.append(duplicate.edits.last());
    require(!ModelEditHistory::inverse(duplicate, uuid(503), &error).has_value(),
            "invalid duplicate-entity source cannot be inverted");
    require(!ModelEditHistory::apply(duplicate, state, &error) && state == stable,
            "invalid event cannot mutate state");

    std::cout << "MODEL HIST-01 canonical inversion checks passed\n";
    return EXIT_SUCCESS;
}
