#ifndef BACKEND_H
#define BACKEND_H

#include <QtCore>
#include <QtGui>
#include <QObject>
#include <QList>
#include <QThread>

#include <hidapi/hidapi.h>

#include "control_midi.h"
#include "tinamicomm.h"

#define BLUEPRINT_LAYOUT_DATA_SIZE 128
#define BLUEPRINT_PRESET_DATA_SIZE 256
#define BLUEPRINT_CONTROL_DATA_SIZE 16
#define BLUEPRINT_CONTROL_TYPE_INDEX 16

class BackEnd : public QObject
{
        Q_OBJECT

        Q_PROPERTY(unsigned char preset READ preset WRITE setPreset NOTIFY presetChanged)

        Q_PROPERTY(DeviceStatus deviceStatus READ deviceStatus WRITE setDeviceStatus NOTIFY deviceStatusChanged)

        Q_PROPERTY(unsigned char component READ component WRITE selectComponent NOTIFY componentChanged)
        Q_PROPERTY(ComponentButtonBehaviour componentButtonBehaviour READ componentButtonBehaviour WRITE setComponentButtonBehaviour NOTIFY componentButtonBehaviourChanged)
        Q_PROPERTY(ComponentMode componentMode READ componentMode WRITE setComponentMode NOTIFY componentModeChanged)
        Q_PROPERTY(unsigned char componentData READ componentData WRITE setComponentData NOTIFY componentDataChanged)
        Q_PROPERTY(unsigned char componentChannel READ componentChannel WRITE setComponentChannel NOTIFY componentChannelChanged)
        Q_PROPERTY(unsigned char componentMinValue READ componentMinValue WRITE setComponentMinValue NOTIFY componentMinValueChanged)
        Q_PROPERTY(unsigned char componentMaxValue READ componentMaxValue WRITE setComponentMaxValue NOTIFY componentMaxValueChanged)

        Q_PROPERTY(unsigned char control0Type READ control0Type WRITE setControl0Type NOTIFY control0TypeChanged)
        Q_PROPERTY(unsigned char control1Type READ control1Type WRITE setControl1Type NOTIFY control1TypeChanged)
        Q_PROPERTY(unsigned char control2Type READ control2Type WRITE setControl2Type NOTIFY control2TypeChanged)
        Q_PROPERTY(unsigned char control3Type READ control3Type WRITE setControl3Type NOTIFY control3TypeChanged)
        Q_PROPERTY(unsigned char control4Type READ control4Type WRITE setControl4Type NOTIFY control4TypeChanged)
        Q_PROPERTY(unsigned char control5Type READ control5Type WRITE setControl5Type NOTIFY control5TypeChanged)
        Q_PROPERTY(unsigned char control6Type READ control6Type WRITE setControl6Type NOTIFY control6TypeChanged)
        Q_PROPERTY(unsigned char control7Type READ control7Type WRITE setControl7Type NOTIFY control7TypeChanged)
        Q_PROPERTY(unsigned char control8Type READ control8Type WRITE setControl8Type NOTIFY control8TypeChanged)
        Q_PROPERTY(unsigned char control9Type READ control9Type WRITE setControl9Type NOTIFY control9TypeChanged)
        Q_PROPERTY(unsigned char control10Type READ control10Type WRITE setControl10Type NOTIFY control10TypeChanged)
        Q_PROPERTY(unsigned char control11Type READ control11Type WRITE setControl11Type NOTIFY control11TypeChanged)
        Q_PROPERTY(unsigned char control12Type READ control12Type WRITE setControl12Type NOTIFY control12TypeChanged)
        Q_PROPERTY(unsigned char control13Type READ control13Type WRITE setControl13Type NOTIFY control13TypeChanged)
        Q_PROPERTY(unsigned char control14Type READ control14Type WRITE setControl14Type NOTIFY control14TypeChanged)
        Q_PROPERTY(unsigned char control15Type READ control15Type WRITE setControl15Type NOTIFY control15TypeChanged)

        Q_PROPERTY(bool synchronizing READ synchronizing WRITE setSynchronizing NOTIFY synchronizingChanged)

public:
        explicit BackEnd(QObject *parent = nullptr);
        ~BackEnd();

        enum DeviceStatus{
            Unplugged,
            Ready_for_config,
            Ready_for_update,
            Wrong_data,
            Ok_data,
            Component,
            Working,
        };

        Q_ENUM(DeviceStatus)

        enum ConfigStatus{
            Request,
            WaitOK,
            WaitFinish,
        };

        Q_ENUM(ConfigStatus)

        enum ReadPresetStatus{
            Request_Preset,
            WaitOK_Preset,
            WaitFinish_Preset,
        };

        Q_ENUM(ReadPresetStatus)

        enum SyncStatus{
            Request_Sync,
            WaitOK_Sync,
            SendLayout_Sync,
            SendFinish_Sync,
            SendReqPreset_Sync,
            WaitOK1_Sync,
            SendPreset_Sync,
            SendFinish1_Sync,
        };

        Q_ENUM(SyncStatus)

        enum ComponentMode{
                VoiceNote,
                VoicePolyPressure,
                VoiceControlChange,
                VoiceProgramChange,
                VoiceChannelPreassure,
                VoicePitchBend,
        };
        Q_ENUM(ComponentMode)


        enum ComponentButtonBehaviour
        {
                None,
                Momentary,
                Toggle,
        };
        Q_ENUM(ComponentButtonBehaviour)

        enum ErrorCode
        {
                Success = 0,
                NotConnected,
                Fail,
                IncorrectCommand,
                Timeout,
                Other = 0xFF
        };
        Q_ENUM(ErrorCode)

        union ComponentConfig{
                unsigned char config[16];
                struct {
                        uint8_t mode; //  Especifica tipo de control, puede ser pulsante, switch, pot, slider
                        uint8_t channel; //
                        uint8_t data; //
                        uint8_t min; //
                        uint8_t max; //
                        uint8_t presetValue; //
                        uint8_t reserved1;
                        uint8_t reserved2;
                        uint8_t reserved3;
                        uint8_t reserved4;
                        uint8_t reserved5;
                        uint8_t reserved6;
                        uint8_t reserved7;
                        uint8_t reserved8;
                        uint8_t reserved9;
                        uint8_t reserved10;
                } bytes;
        };

        struct PacketConfig {
                unsigned char data[64];
        };

        union PresetConfig {
                PacketConfig packet[4];
                ComponentConfig component[16];
        };

        struct Md1Configuration {
                PresetConfig preset[16];
        };

        Md1Configuration configuration;

        DeviceStatus deviceStatus() { return m_deviceStatus; }

        ComponentMode componentMode() const { return m_componentMode; }

        unsigned char preset() const {return m_preset;}

        //unsigned char component() const {return m_component;}

        //   Control_midi control() {return  m_control;}

        bool set_layout;

        unsigned char layout[BLUEPRINT_LAYOUT_DATA_SIZE];

        unsigned char componentData() const { return m_componentData;}

        unsigned char componentChannel() const { return m_componentChannel;}

        unsigned char componentMinValue() const { return m_componentMinValue;}

        unsigned char componentMaxValue() const { return m_componentMaxValue;}

        ComponentButtonBehaviour componentButtonBehaviour() const { return m_componentButtonBehaviour;}

        unsigned char component() const { return m_component;}

        unsigned char control0Type() const { return m_controlType[0];}
        unsigned char control1Type() const { return m_controlType[1];}
        unsigned char control2Type() const { return m_controlType[2];}
        unsigned char control3Type() const { return m_controlType[3];}
        unsigned char control4Type() const { return m_controlType[4];}
        unsigned char control5Type() const { return m_controlType[5];}
        unsigned char control6Type() const { return m_controlType[6];}
        unsigned char control7Type() const { return m_controlType[7];}
        unsigned char control8Type() const { return m_controlType[8];}
        unsigned char control9Type() const { return m_controlType[9];}
        unsigned char control10Type() const { return m_controlType[10];}
        unsigned char control11Type() const { return m_controlType[11];}
        unsigned char control12Type() const { return m_controlType[12];}
        unsigned char control13Type() const { return m_controlType[13];}
        unsigned char control14Type() const { return m_controlType[14];}
        unsigned char control15Type() const { return m_controlType[15];}

        bool synchronizing() {return m_sync;}

        Control_midi *this_control = new Control_midi();

        void pollUSB(uint16_t deviceVIDtoPoll, uint16_t devicePIDtoPoll);
        ErrorCode open(uint16_t deviceVIDtoOpen, uint16_t devicePIDtoOpen);

        int timerLoopInterval;
        int timerReadInterval;
        int timerSyncInterval;

public slots:
        void syncHost2Device();

        void selectComponent(const unsigned char &component);

        void setDeviceStatus(const DeviceStatus &deviceStatus);

        void setComponentMode(const ComponentMode &componentMode);

        void setPreset(const unsigned char &preset);

        void setComponentData(const unsigned char &componentData);
        void setComponentChannel(const unsigned char &componentChannel);
        void setComponentMinValue(const unsigned char &componentMinValue);
        void setComponentMaxValue(const unsigned char &componentMaxValue);

        void setComponentButtonBehaviour(const ComponentButtonBehaviour &componentButtonBehaviour);

        void setControl0Type(const unsigned char &controlType);
        void setControl1Type(const unsigned char &controlType);
        void setControl2Type(const unsigned char &controlType);
        void setControl3Type(const unsigned char &controlType);
        void setControl4Type(const unsigned char &controlType);
        void setControl5Type(const unsigned char &controlType);
        void setControl6Type(const unsigned char &controlType);
        void setControl7Type(const unsigned char &controlType);
        void setControl8Type(const unsigned char &controlType);
        void setControl9Type(const unsigned char &controlType);
        void setControl10Type(const unsigned char &controlType);
        void setControl11Type(const unsigned char &controlType);
        void setControl12Type(const unsigned char &controlType);
        void setControl13Type(const unsigned char &controlType);
        void setControl14Type(const unsigned char &controlType);
        void setControl15Type(const unsigned char &controlType);
        void setLayout();

        void setSynchronizing(bool &sync);

private slots:
        void timer_timeout();
        void timerRead_timeout();
        void timerSync_timeout();

signals:
        void deviceStatusChanged();
        void componentChanged();
        void presetChanged();
        void componentModeChanged();
        void componentButtonBehaviourChanged();
        void componentChannelChanged();
        void componentDataChanged();
        void componentMinValueChanged();
        void componentMaxValueChanged();
        void control0TypeChanged();
        void control1TypeChanged();
        void control2TypeChanged();
        void control3TypeChanged();
        void control4TypeChanged();
        void control5TypeChanged();
        void control6TypeChanged();
        void control7TypeChanged();
        void control8TypeChanged();
        void control9TypeChanged();
        void control10TypeChanged();
        void control11TypeChanged();
        void control12TypeChanged();
        void control13TypeChanged();
        void control14TypeChanged();
        void control15TypeChanged();
        void synchronizingChanged();

protected:
        hid_device *md1_device;
        bool deviceConnected;

private:
        DeviceStatus m_deviceStatus;
        ConfigStatus conf_state;
        ReadPresetStatus preset_status;
        SyncStatus sync_status;
        ComponentButtonBehaviour m_componentButtonBehaviour = None;
        ComponentMode m_componentMode;
        unsigned char m_componentData = 0;
        unsigned char m_componentChannel = 0;
        unsigned char m_componentMinValue = 0;
        unsigned char m_componentMaxValue = 0;
        unsigned char m_component = 0;
        unsigned char m_preset = 0;

        unsigned char m_controlType[16];

        unsigned char preset_array[16];

        unsigned char packet_num_buffer;

        bool m_sync;
        /*unsigned char *m_value = new unsigned char [16];*/

        QTimer *timerLoop;
        QTimer *timerRead;
        QTimer *timerSync;

        void senseValue();
        void senseDeviceStatus();
        void readDeviceConfiguration(ConfigStatus &conf_state);

        void readPreset(const unsigned char &preset);
        void redPresetReq(const unsigned char &preset);
        void WaitOKPreset();
        void WaitFinishPreset(const unsigned char &preset);

        void syncReq();
        void WaitOKSync();
        void SendLayoutSync();
        void SendFinishSync();
        void SendReqPresetSync();
        void WaitOK1Sync();
        void SendPresetSync();
        void SendFinish1Sync();

        std::vector <unsigned char> change_index;

        QList<Control_midi*> controls;
};

#endif // BACKEND_H
