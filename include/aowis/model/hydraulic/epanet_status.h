#ifndef EPANET_STATUS_H
#define EPANET_STATUS_H

#include <QMetaType>
#include <QObject>
#include <QString>
#include <QStringList>

class EpanetEnums
{
    Q_GADGET

public:
    enum class Stage
    {
        None,

        CreateProject,
        InitializeProject,
        ConfigureOptions,

        BuildNetwork,
        AddPattern,
        AddCurve,
        AddReservoir,
        AddJunction,
        AddTank,
        AddPipe,
        AddPump,
        AddValve,
        AddControl,
        AddRule,

        RunHydraulics,
        RunQuality,

        ReadResults,
        ReadJunctionResults,
        ReadReservoirResults,
        ReadTankResults,
        ReadPipeResults,
        ReadPumpResults,
        ReadValveResults,
        ReadStatistics,
        ReadEnergyUsage,

        CloseHydraulics,
        CloseQuality,
        SaveHydraulics,
        GenerateReport,
        Cleanup
    };
    Q_ENUM(Stage)

    enum class Operation
    {
        None,

        EN_createproject,
        EN_deleteproject,

        EN_init,
        EN_settitle,
        EN_settimeparam,
        EN_setoption,
        EN_setdemandmodel,
        EN_setqualtype,

        EN_addpattern,
        EN_getpatternindex,
        EN_setpattern,

        EN_addcurve,
        EN_getcurveindex,
        EN_setcurve,
        EN_setcurvetype,

        EN_addnode,
        EN_setnodevalue,
        EN_settankdata,
        EN_setjuncdata,
        EN_adddemand,
        EN_setdemandname,
        EN_setcoord,

        EN_addlink,
        EN_setpipedata,
        EN_setlinkvalue,
        EN_setheadcurveindex,
        EN_setvertices,

        EN_setcomment,
        EN_settag,

        EN_addcontrol,
        EN_setcontrolenabled,

        EN_addrule,
        EN_setrulepriority,
        EN_setruleenabled,

        EN_setreport,
        EN_setstatusreport,
        EN_setreportcallback,
        EN_setreportcallbackuserdata,

        EN_openH,
        EN_initH,
        EN_runH,
        EN_nextH,
        EN_closeH,
        EN_saveH,

        EN_openQ,
        EN_initQ,
        EN_runQ,
        EN_nextQ,
        EN_stepQ,
        EN_closeQ,

        EN_report,

        EN_getnodeindex,
        EN_getnodevalue,
        EN_getlinkindex,
        EN_getlinkvalue,
        EN_getstatistic,
        EN_geterror
    };
    Q_ENUM(Operation)

    enum class Property
    {
        None,

        Elevation,
        Demand,
        DemandDeficit,
        Emitter,
        Leakage,

        Head,
        Pressure,
        Level,
        Volume,

        Flow,
        Velocity,
        Headloss,

        Quality,
        SourceMass,

        Status,
        Setting,
        Energy,
        Efficiency,

        InitialStatus
    };
    Q_ENUM(Property)

    enum class EntityType
    {
        None,

        Project,
        Network,

        Node,
        Junction,
        Reservoir,
        Tank,

        Link,
        Pipe,
        Pump,
        Valve,

        Pattern,
        Curve,
        Control,
        Rule,

        HydraulicSolver,
        QualitySolver,
        Report,
        Result
    };
    Q_ENUM(EntityType)
};

using EpanetStage = EpanetEnums::Stage;
using EpanetOperation = EpanetEnums::Operation;
using EpanetProperty = EpanetEnums::Property;
using EpanetEntityType = EpanetEnums::EntityType;

struct EpanetEntity
{
    EpanetEntityType type = EpanetEntityType::None;

    QString id;
    int index = 0;
};

struct EpanetStatus
{
    bool success = true;

    int epanet_error_code = 0;

    EpanetStage stage = EpanetStage::None;
    EpanetOperation operation = EpanetOperation::None;
    EpanetProperty property = EpanetProperty::None;

    EpanetEntity entity;

    QString message;
    QString message_epanet;
    QStringList details;
};

Q_DECLARE_METATYPE(EpanetStatus)

#endif // EPANET_STATUS_H
