#ifndef BACKEND_H
#define BACKEND_H

#include <QtCore>
#include <QtGui>
#include <QObject>
#include <QList>
#include <QThread>
#include <QVariant>

#include "RtMidi.h"

#include "control_midi.h"
#include "tinamicomm.h"

#define BLUEPRINT_LAYOUT_DATA_SIZE 128
#define BLUEPRINT_PRESET_DATA_SIZE 256
#define BLUEPRINT_CONTROL_DATA_SIZE 16
#define BLUEPRINT_CONTROL_TYPE_INDEX 16
#define BLUEPRINT_PRESET_GLOBAL_CHANNEL_INDEX 16

    class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DeviceStatus deviceStatus READ deviceStatus WRITE setDeviceStatus NOTIFY deviceStatusChanged)

        Q_PROPERTY(unsigned char preset READ preset WRITE setPreset NOTIFY presetChanged)

        Q_PROPERTY(unsigned char component READ component WRITE selectComponent NOTIFY componentChanged)
            Q_PROPERTY(ComponentButtonBehaviour componentButtonBehaviour READ componentButtonBehaviour WRITE setComponentButtonBehaviour NOTIFY componentButtonBehaviourChanged)
                Q_PROPERTY(ComponentMode componentMode READ componentMode WRITE setComponentMode NOTIFY componentModeChanged)
                    Q_PROPERTY(unsigned char componentData READ componentData WRITE setComponentData NOTIFY componentDataChanged)
                        Q_PROPERTY(unsigned char componentChannel READ componentChannel WRITE setComponentChannel NOTIFY componentChannelChanged)
                            Q_PROPERTY(unsigned char globalChannel READ globalChannel WRITE setGlobalChannel NOTIFY globalChannelChanged)
                                Q_PROPERTY(unsigned char componentMinValue READ componentMinValue WRITE setComponentMinValue NOTIFY componentMinValueChanged)
                                    Q_PROPERTY(unsigned char componentMaxValue READ componentMaxValue WRITE setComponentMaxValue NOTIFY componentMaxValueChanged)

        Q_PROPERTY(QList<QVariant> controlType READ controlType WRITE SetControlType NOTIFY controlTypeChanged)

        Q_PROPERTY(bool synchronizing READ synchronizing WRITE setSynchronizing NOTIFY synchronizingChanged)

        public:
        explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd();

        enum DeviceStatus{
            Unplugged,
            Pull_Layout,
            Push_Layout,
            Pull_Preset,
            Push_Preset,
            Idle,
            Ready_for_update,
            Ok_data,
            Component,
            };
    Q_ENUM(DeviceStatus)

        enum PullLayoutStatus{
            PullLayoutReq,
            PullLayoutCheck,
            };

        Q_ENUM(PullLayoutStatus)

        enum PullPresetStatus{
            PullPresetReq,
            PullPresetCheck,
            };

        Q_ENUM(PullPresetStatus)

        enum PushLayoutStatus{
            PushLayoutReq,
            PushLayoutCheck,
            };

        Q_ENUM(PushLayoutStatus)

        enum PushPresetStatus{
            PushPresetReq,
            PushPresetCheck,
            };

        Q_ENUM(PushPresetStatus)

        enum SyncStatus{
            Request_Sync,
            SendLayout_Sync,
            SendFinish_Sync,
            SendReqPreset_Sync,
            //WaitOK1_Sync,
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

    enum ComponentConfiguration{
        NoneConfiguration,
        Button,
        Pot
    };
    Q_ENUM(ComponentConfiguration)

        enum ComponentButtonBehaviour {
            NoneBehaviour,
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
            uint8_t config;
            uint8_t mode;
            uint8_t channel;
            uint8_t data;
            uint8_t min;
            uint8_t max;
            uint8_t presetValue;
            uint8_t reserved1;
            uint8_t reserved2;
            uint8_t reserved3;
            uint8_t reserved4;
            uint8_t reserved5;
            uint8_t reserved6;
            uint8_t reserved7;
            uint8_t reserved8;
            uint8_t reserved9;
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

    unsigned char globalChannel() const { return m_globalChannel;}

    unsigned char componentMinValue() const { return m_componentMinValue;}

    unsigned char componentMaxValue() const { return m_componentMaxValue;}

    ComponentButtonBehaviour componentButtonBehaviour() const { return m_componentButtonBehaviour;}

    unsigned char component() const { return m_component;}

    QList<QVariant> controlType() { return m_controlType;}

    bool synchronizing() {return m_sync;}

    Control_midi *this_control = new Control_midi();

    int timerLoopInterval;

    bool sysExCmdDone;

    void sysExSendMessage(uint8_t command, uint8_t length, uint8_t *buffer);
    void sysExSendMessage(uint8_t command);

public slots:
    void syncHost2Device();

    void selectComponent(const unsigned char &component);

    void setDeviceStatus(const DeviceStatus &deviceStatus);

    void setComponentMode(const ComponentMode &componentMode);

    void setPreset(const unsigned char &preset);

    void setComponentData(const unsigned char &componentData);
    void setComponentChannel(const unsigned char &componentChannel);
    void setGlobalChannel(const unsigned char &globalChannel);
    void setComponentMinValue(const unsigned char &componentMinValue);
    void setComponentMaxValue(const unsigned char &componentMaxValue);

    void setComponentButtonBehaviour(const ComponentButtonBehaviour &componentButtonBehaviour);

    void SetControlType(QList<QVariant> &controlType);

    void setLayout();

    void setSynchronizing(bool &sync);

private slots:
    void main_state_machine();

signals:
    void deviceStatusChanged();
    void componentChanged();
    void presetChanged();
    void componentModeChanged();
    void componentButtonBehaviourChanged();
    void componentChannelChanged();
    void globalChannelChanged();
    void componentDataChanged();
    void componentMinValueChanged();
    void componentMaxValueChanged();
    void controlTypeChanged();
    void synchronizingChanged();
    void openError();

protected:
    bool deviceConnected;

private:
    DeviceStatus m_deviceStatus;

    PushLayoutStatus push_layout_status;
    PullLayoutStatus layout_status;
    PushPresetStatus push_preset_status;
    PullPresetStatus preset_status;

    SyncStatus sync_status;
    ComponentButtonBehaviour m_componentButtonBehaviour = NoneBehaviour;
    ComponentMode m_componentMode;
    unsigned char m_componentData = 0;
    unsigned char m_componentChannel = 0;
    unsigned char m_globalChannel = 0;
    unsigned char m_componentMinValue = 0;
    unsigned char m_componentMaxValue = 0;
    unsigned char m_component = 0;
    unsigned char m_preset = 0;

    QList<QVariant> m_controlType = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    unsigned char preset_array[16];

    unsigned char packet_num_buffer;

    bool m_sync;
    /*unsigned char *m_value = new unsigned char [16];*/

    QTimer *timerLoop;

    void searchDevice();
    void PullLayout();
    void PushLayout();
    void PullPreset(const unsigned char &preset);
    void PushPreset(const unsigned char &preset);

    void readPreset(const unsigned char &preset);

    std::vector <unsigned char> change_index;

    QList<Control_midi*> controls;

    RtMidiOut *midiout;
    RtMidiIn *midiin;
};

#endif // BACKEND_H
