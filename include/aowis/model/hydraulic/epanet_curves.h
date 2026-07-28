#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_CURVES_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_CURVES_H

#include <QList>
#include <QString>
#include <QUuid>

struct EpanetTimePattern
{
    QString id;
    QUuid uuid;
    QList<double> factors;
    QString comment;
};

struct TankVolumeCurvePoint
{
    double level_m = 0.0;
    double volume_m3 = 0.0;
};

struct TankVolumeCurve
{
    QString id;
    QUuid uuid;
    QList<TankVolumeCurvePoint> points;
    QString comment;
};

struct PumpCurvePoint
{
    double flow_m3_per_h = 0.0;
    double head_m = 0.0;
};

struct PumpCurve
{
    QString id;
    QUuid uuid;
    QList<PumpCurvePoint> points;
    QString comment;
};

using PumpHeadCurvePoint = PumpCurvePoint;
using PumpHeadCurve = PumpCurve;

struct PumpEfficiencyCurvePoint
{
    double flow_m3_per_h = 0.0;
    double efficiency_percent = 0.0;
};

struct PumpEfficiencyCurve
{
    QString id;
    QUuid uuid;
    QList<PumpEfficiencyCurvePoint> points;
    QString comment;
};

struct ValveHeadlossCurvePoint
{
    double flow_m3_per_h = 0.0;
    double head_loss_m = 0.0;
};

struct ValveHeadlossCurve
{
    QString id;
    QUuid uuid;
    QList<ValveHeadlossCurvePoint> points;
    QString comment;
};

struct ValveCharacteristicCurvePoint
{
    double position_percent = 0.0;
    double relative_flow_percent = 0.0;
};

struct ValveCharacteristicCurve
{
    QString id;
    QUuid uuid;
    QList<ValveCharacteristicCurvePoint> points;
    QString comment;
};

struct GenericCurvePoint
{
    double x = 0.0;
    double y = 0.0;
};

struct GenericCurve
{
    QString id;
    QUuid uuid;
    QList<GenericCurvePoint> points;
    QString comment;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_CURVES_H
