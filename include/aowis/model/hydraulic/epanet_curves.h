#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_CURVES_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_CURVES_H

#include <QList>
#include <QString>
#include <QUuid>

struct EpanetPatternTime
{
    QString id;
    QUuid uuid;
    QList<double> factors;
    QString comment;
};

struct EpanetCurveTankVolumePoint
{
    double water_level_m = 0.0;
    double volume_m3 = 0.0;
};

struct EpanetCurveTankVolume
{
    QString id;
    QUuid uuid;
    QList<EpanetCurveTankVolumePoint> points;
    QString comment;
};

struct EpanetCurvePumpHeadPoint
{
    double flow_m3_per_h = 0.0;
    double head_gain_m = 0.0;
};

struct EpanetCurvePumpHead
{
    QString id;
    QUuid uuid;
    QList<EpanetCurvePumpHeadPoint> points;
    QString comment;
};

struct EpanetCurvePumpEfficiencyPoint
{
    double flow_m3_per_h = 0.0;
    double efficiency_percent = 0.0;
};

struct EpanetCurvePumpEfficiency
{
    QString id;
    QUuid uuid;
    QList<EpanetCurvePumpEfficiencyPoint> points;
    QString comment;
};

struct EpanetCurveValveHeadlossPoint
{
    double flow_m3_per_h = 0.0;
    double head_loss_m = 0.0;
};

struct EpanetCurveValveHeadloss
{
    QString id;
    QUuid uuid;
    QList<EpanetCurveValveHeadlossPoint> points;
    QString comment;
};

struct EpanetCurveValveCharacteristicPoint
{
    double position_percent = 0.0;
    double relative_flow_percent = 0.0;
};

struct EpanetCurveValveCharacteristic
{
    QString id;
    QUuid uuid;
    QList<EpanetCurveValveCharacteristicPoint> points;
    QString comment;
};

struct EpanetCurveGenericPoint
{
    double x = 0.0;
    double y = 0.0;
};

struct EpanetCurveGeneric
{
    QString id;
    QUuid uuid;
    QList<EpanetCurveGenericPoint> points;
    QString comment;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_CURVES_H
