#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>

#include <QThread>
#include <QObject>
#include <QDebug>
#include <QQuickWindow>

#include "control_midi.h"
#include "backend.h"
#include "defs.h"

#define BLUEPRINT_USB_DATA_PACKET_SIZE 64
#define BLUEPRINT_USB_HID_PACKET_SIZE (BLUEPRINT_USB_DATA_PACKET_SIZE + 1)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             BACKEND                                                                 //
/// \brief BackEnd::BackEnd                                                                                             //
/// \param parent                                                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BackEnd::BackEnd(QObject *parent) :
        QObject(parent)
{
        set_layout = false;          //  Bool variable used only to known if the tinami is conected for the first time

        setDeviceStatus(Unplugged);       //  Select Variable. Values: "unplugged"; "connected"; "wrongData"; "okData"; "working"
        conf_state = Request;
        preset_status = Request_Preset;
        sync_status = Request_Sync;
        memset(layout, 0, sizeof (layout));
        packet_num_buffer = 0;
        setComponentMinValue(0);
        setComponentMaxValue(127);

        m_sync = false;

        memset(&configuration, 0, sizeof (configuration));
        memset(&preset_array, 0, sizeof (preset_array));

        this->timerLoop = new QTimer(this);
        connect(timerLoop, &QTimer::timeout, this, &BackEnd::timer_timeout);
        timerLoopInterval = 100;
        this->timerLoop->start(timerLoopInterval);

        this->timerRead = new QTimer(this);
        connect(timerRead, &QTimer::timeout, this, &BackEnd::timerRead_timeout);
        timerReadInterval = 100;

        this->timerSync = new QTimer(this);
        connect(timerSync, &QTimer::timeout, this, & BackEnd::timerSync_timeout);
        timerSyncInterval = 100;
}

BackEnd::~BackEnd()
{
        this->timerLoop->stop();

        if(m_deviceStatus != Unplugged){
                hid_close(md1_device);
                qDebug() << "Released Interface\n";
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     TIMER OVERFLOW MAIN LOOP                                                        //
/// \brief BackEnd::timer_timeout                                                                                       //
///                                                                                                                     //
/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BackEnd::timer_timeout(){
        switch (m_deviceStatus) {
        case Unplugged:
                senseDeviceStatus();
                break;
        case Ready_for_config:
                readDeviceConfiguration(conf_state);
                break;
        case Ready_for_update:
                break;
        case Ok_data:
                setLayout();
                break;
        case Component:
                setPreset(0);
                readPreset(0);
                setDeviceStatus(Working);
                break;
        case Working:
                senseValue();
                break;
        case Wrong_data:
                readDeviceConfiguration(conf_state);
                break;
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      TIMER TO READ OVERFLOW                                                         //
/// \brief BackEnd::timer_timeout                                                                                       //
///                                                                                                                     //
/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BackEnd::timerRead_timeout(){
        switch (preset_status) {
        case Request_Preset:
                redPresetReq(m_preset);
                break;
        case WaitOK_Preset:
                WaitOKPreset();
                break;
        case WaitFinish_Preset:
                WaitFinishPreset(m_preset);
                break;

        }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      TIMER SYNC OVERFLOW                                                         //
/// \brief BackEnd::timersYNC_timeout                                                                                       //
///                                                                                                                     //
/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BackEnd::timerSync_timeout(){
        switch (sync_status) {
        case Request_Sync:
                syncReq();
                break;
        case WaitOK_Sync:
                WaitOKSync();
                break;
        case SendLayout_Sync:
                SendLayoutSync();
                break;
        case SendFinish_Sync:
                SendFinishSync();
                break;
        case SendReqPreset_Sync:
                SendReqPresetSync();
                break;
        case WaitOK1_Sync:
                WaitOK1Sync();
                break;
        case SendPreset_Sync:
                SendPresetSync();
                break;
        case SendFinish1_Sync:
                SendFinish1Sync();
                break;
        }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       DEVICE STATUS                                                                 //
/// \brief BackEnd::senseDeviceStatus                                                                                   //
/// \param parent                                                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BackEnd::senseDeviceStatus(){

        this->timerLoop->stop();
        setDeviceStatus(Unplugged);

        pollUSB(0x04D8, 0x0053);
        if(deviceConnected){
                qWarning("Attempting to open device...");
                open(0x04D8, 0x0053);
                setDeviceStatus(Ready_for_config);
        }

        pollUSB(0x04D8, 0x003C);
        if(deviceConnected){
                qWarning("Attempting to open device to bootload...");
                open(0x04D8, 0x003C);
                setDeviceStatus(Ready_for_update);
        }

        this->timerLoop->start(timerLoopInterval);
        return;
}

void BackEnd::setDeviceStatus(const DeviceStatus &deviceStatus){

        if (deviceStatus == m_deviceStatus)
                return;

        m_deviceStatus = deviceStatus;
        emit deviceStatusChanged();
        return;
}

void BackEnd::pollUSB(uint16_t deviceVIDtoPoll, uint16_t devicePIDtoPoll)
{
        hid_device_info *dev;

        dev = hid_enumerate(deviceVIDtoPoll, devicePIDtoPoll);

        deviceConnected = (dev != nullptr);
        hid_free_enumeration(dev);
}

BackEnd::ErrorCode BackEnd::open(uint16_t deviceVIDtoOpen, uint16_t devicePIDtoOpen)
{
        md1_device = hid_open(deviceVIDtoOpen, devicePIDtoOpen, nullptr);
        if(md1_device){
                deviceConnected = true;
                hid_set_nonblocking(md1_device, true);
                qInfo("Device successfully connected to.");
                return Success;
        }

        qWarning("Unable to open device.");
        return NotConnected;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       DEVICE CONFIGURATION                                                          //
/// \brief BackEnd::senseDeviceStatus                                                                                   //
///
///  HOST REQUEST READING
//
//   ___________________________ ___________________________ ___________________________ ___________________________
//  |           SOF             |   HOST_REQUIRES_READING   |           CMD             |   HOST_REQUIRES_READING   |
//   --------------------------- --------------------------- --------------------------- ---------------------------
//
//  HOST REQUEST READING
//
//   ___________________________ ___________________________ ___________________________ ___________________________
//  |           SOF             |   HOST_REQUIRES_READING   |           CMD             |   HOST_REQUIRES_READING   |
//   --------------------------- --------------------------- --------------------------- ---------------------------
//
//  HOST REQUEST READING
//
//   ___________________________ ___________________________ ___________________________ ___________________________
//  |           SOF             |   HOST_REQUIRES_READING   |           CMD             |   HOST_REQUIRES_READING   |
//   --------------------------- --------------------------- --------------------------- ---------------------------                                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BackEnd::readDeviceConfiguration(ConfigStatus &conf_state){

        unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
        unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read

        qDebug() << conf_state;

        switch (conf_state) {
        case Request:
                memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);
                qDebug() << "\nReading device configuration\n";
                this->timerLoop->stop();
                data_out[0]=0; data_out[1]=SOF; data_out[2]=HOST_REQUIRES_READING; data_out[3]=CMD_LAYOUT; data_out[4]=0;

                hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE);
                qDebug() << "Read request send";
                if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
                        setDeviceStatus(Unplugged);
                        conf_state = Request;
                        qDebug() << "read error";
                        this->timerLoop->start(timerLoopInterval);
                        return;
                }
                conf_state = WaitFinish;//conf_state = WaitOK;
                packet_num_buffer = 0;
                this->timerLoop->start(100);
                break;

        case WaitOK:
                this->timerLoop->stop();
                qDebug() << "#";
                memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);
                if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
                        setDeviceStatus(Unplugged);
                        conf_state = Request;
                        qDebug() << "read error";
                        this->timerLoop->start(timerLoopInterval);
                        return;
                }
                if ((data_in[0] != SOF) && (data_in[1] != OK) && (data_in[2] != CMD_LAYOUT)){
                        qDebug() << "Ok";
                        conf_state = WaitFinish;
                }
                packet_num_buffer = 0;
                this->timerLoop->start(timerLoopInterval);
                break;

        case WaitFinish:
                this->timerLoop->stop();
                qDebug() << "Reading Data";
                int offset;

                memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);
                if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
                        qDebug() << " write error";
                        setDeviceStatus(Unplugged);
                        conf_state = Request;
                        this->timerLoop->start(timerLoopInterval);
                        return;
                }

                if(data_in[1] == FINISH){
                        setDeviceStatus(Ok_data);
                        conf_state = Request;
                        qDebug() << "Done";
                        this->timerLoop->start(timerLoopInterval);
                        break;
                }

                offset = packet_num_buffer * BLUEPRINT_USB_DATA_PACKET_SIZE;

                for (int i = 0; i < BLUEPRINT_USB_DATA_PACKET_SIZE; i++) {
                        layout[offset + i] = data_in[i];
                        qDebug() << offset + i << layout[offset + i];
                }

                packet_num_buffer = packet_num_buffer+1;
                this->timerLoop->start(timerLoopInterval);
                break;
        }
        return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     SET LAYOUT
/// \brief BackEnd::setLayout
///                                     ENHANCE
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BackEnd::setLayout(){

        this->timerLoop->stop();

        setControl0Type(layout[0]); qDebug() << "#";
        setControl1Type(layout[1]); qDebug() << "#";
        setControl2Type(layout[2]); qDebug() << "#";
        setControl3Type(layout[3]); qDebug() << "#";
        setControl4Type(layout[4]); qDebug() << "#";
        setControl5Type(layout[5]); qDebug() << "#";
        setControl6Type(layout[6]); qDebug() << "#";
        setControl7Type(layout[7]); qDebug() << "#";
        setControl8Type(layout[8]); qDebug() << "#";
        setControl9Type(layout[9]); qDebug() << "#";
        setControl10Type(layout[10]); qDebug() << "#";
        setControl11Type(layout[11]); qDebug() << "#";
        setControl12Type(layout[12]); qDebug() << "#";
        setControl13Type(layout[13]); qDebug() << "#";
        setControl14Type(layout[14]); qDebug() << "#";
        setControl15Type(layout[15]); qDebug() << "#";

        if(!set_layout){
                set_layout = true;
                //sleep(1);
        }

        qDebug() << "Done\n";
        setDeviceStatus(Component);
        this->timerLoop->start(timerLoopInterval);
        return;
}

///////////////////////////////////////////////////////////////
/// \brief BackEnd::setControlType
/// \param controlType
///////////////////////////////////////////////////////////////

void BackEnd::setControl0Type(const unsigned char &controlType){
        if (controlType == m_controlType[0])
                return;
        m_controlType[0] = controlType;
        emit control0TypeChanged();
        return;
}

void BackEnd::setControl1Type(const unsigned char &controlType){
        if (controlType == m_controlType[1])
                return;
        m_controlType[1] = controlType;
        emit control1TypeChanged();
        return;
}

void BackEnd::setControl2Type(const unsigned char &controlType){
        if (controlType == m_controlType[2])
                return;
        m_controlType[2] = controlType;
        emit control2TypeChanged();
        return;
}

void BackEnd::setControl3Type(const unsigned char &controlType){
        if (controlType == m_controlType[3])
                return;
        m_controlType[3] = controlType;
        emit control3TypeChanged();
        return;
}

void BackEnd::setControl4Type(const unsigned char &controlType){
        if (controlType == m_controlType[4])
                return;
        m_controlType[4] = controlType;
        emit control4TypeChanged();
        return;
}

void BackEnd::setControl5Type(const unsigned char &controlType){
        if (controlType == m_controlType[5])
                return;
        m_controlType[5] = controlType;
        emit control5TypeChanged();
        return;
}

void BackEnd::setControl6Type(const unsigned char &controlType){
        if (controlType == m_controlType[6])
                return;
        m_controlType[6] = controlType;
        emit control6TypeChanged();
        return;
}

void BackEnd::setControl7Type(const unsigned char &controlType){
        if (controlType == m_controlType[7])
                return;
        m_controlType[7] = controlType;
        emit control7TypeChanged();
        return;
}

void BackEnd::setControl8Type(const unsigned char &controlType){
        if (controlType == m_controlType[8])
                return;
        m_controlType[8] = controlType;
        emit control8TypeChanged();
        return;
}

void BackEnd::setControl9Type(const unsigned char &controlType){
        if (controlType == m_controlType[9])
                return;
        m_controlType[9] = controlType;
        emit control9TypeChanged();
        return;
}


void BackEnd::setControl10Type(const unsigned char &controlType){
        if (controlType == m_controlType[10])
                return;
        m_controlType[10] = controlType;
        emit control10TypeChanged();
        return;
}


void BackEnd::setControl11Type(const unsigned char &controlType){
        if (controlType == m_controlType[11])
                return;
        m_controlType[11] = controlType;
        emit control11TypeChanged();
        return;
}


void BackEnd::setControl12Type(const unsigned char &controlType){
        if (controlType == m_controlType[12])
                return;
        m_controlType[12] = controlType;
        emit control12TypeChanged();
        return;
}


void BackEnd::setControl13Type(const unsigned char &controlType){
        if (controlType == m_controlType[13])
                return;
        m_controlType[13] = controlType;
        emit control13TypeChanged();
        return;
}


void BackEnd::setControl14Type(const unsigned char &controlType){
        if (controlType == m_controlType[14])
                return;
        m_controlType[14] = controlType;
        emit control14TypeChanged();
        return;
}

void BackEnd::setControl15Type(const unsigned char &controlType){
        if (controlType == m_controlType[15])
                return;
        m_controlType[15] = controlType;
        emit control15TypeChanged();
        return;
}

void BackEnd::setSynchronizing(bool &sync){
        if (sync == m_sync)
                return;
        m_sync = sync;
        emit synchronizingChanged();
        return;
}

///////////////////////////////////////////////////////////////
///
///             Attributes
///
///////////////////////////////////////////////////////////////


void BackEnd::setPreset(const unsigned char &preset){
        if (preset == m_preset)
                return;
        m_preset = preset;
        bool already_read = false;
        for (int i = 0; i < 16; ++i) {
                if (m_preset == preset_array[i])
                        already_read = true;
        }
        if (!already_read){
                readPreset(m_preset);
                for (int i = 1; i < 16; ++i) {
                        if (preset_array[i] == 0){
                                preset_array[i] = m_preset;
                                break;
                        }
                }
        }
        selectComponent(0);
        emit presetChanged();
        emit control0TypeChanged();
        emit control1TypeChanged();
        emit control2TypeChanged();
        emit control3TypeChanged();
        emit control4TypeChanged();
        emit control5TypeChanged();
        emit control6TypeChanged();
        emit control7TypeChanged();
        emit control8TypeChanged();
        emit control9TypeChanged();
        emit control10TypeChanged();
        emit control11TypeChanged();
        emit control12TypeChanged();
        emit control13TypeChanged();
        emit control14TypeChanged();
        emit control15TypeChanged();
}

void BackEnd::setComponentMode(const ComponentMode &deviceMode){
        int index = BLUEPRINT_PRESET_DATA_SIZE * m_preset + BLUEPRINT_CONTROL_DATA_SIZE * m_component;

        qDebug() << index << m_preset << m_component << deviceMode;

        if (deviceMode == m_componentMode)
                return;

        configuration.preset[m_preset].component[m_component].bytes.mode = static_cast<unsigned char>(deviceMode);

        m_componentMode = deviceMode;

        emit componentModeChanged();
        return;
}

void BackEnd::setComponentChannel(const unsigned char &controlChannel){
        unsigned char channel = 0xFF;

        if(controlChannel == m_componentChannel)
                return;

        if ((controlChannel >= 1) && (controlChannel <= 16)){
                channel = controlChannel - 1;
                m_componentChannel = controlChannel;
        }

        configuration.preset[m_preset].component[m_component].bytes.channel = channel;

        if (channel == 0xFF)
                m_componentChannel = 0;

        emit componentChannelChanged();
        return;
}

void BackEnd::setComponentData(const unsigned char &controlData){
        if(controlData == m_componentData)
                return;

        configuration.preset[m_preset].component[m_component].bytes.data = controlData;
        m_componentData = controlData;

        emit componentDataChanged();
        return;
}

void BackEnd::setComponentMinValue(const unsigned char &minValue){
        if (minValue == m_componentMinValue)
                return;

        configuration.preset[m_preset].component[m_component].bytes.min = minValue;
        m_componentMinValue = minValue;

        emit componentMinValueChanged();
        return;
}

void BackEnd::setComponentMaxValue(const unsigned char &maxValue){
        if (maxValue == m_componentMaxValue)
                return;

        configuration.preset[m_preset].component[m_component].bytes.max = maxValue;
        m_componentMaxValue = maxValue;

        emit componentMaxValueChanged();
        return;
}

void BackEnd::setComponentButtonBehaviour(const ComponentButtonBehaviour &controlButtonBehaviour){
        int index = BLUEPRINT_CONTROL_DATA_SIZE + m_component;

        if (controlButtonBehaviour == m_componentButtonBehaviour)
                return;

        switch (controlButtonBehaviour) {
        case None:
                break;
        case Momentary:
                layout[index] = Momentary;
                break;
        case Toggle:
                layout[index] = Toggle;
                break;
        }

        m_componentButtonBehaviour = controlButtonBehaviour;
        emit componentButtonBehaviourChanged();
        return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     SELECT COMPONENT
/// \brief BackEnd::selectComponent
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void BackEnd::selectComponent(const unsigned char &component){
        unsigned char type;
        unsigned char mode;
        unsigned char channel;
        unsigned char data;
        unsigned char minValue;
        unsigned char maxValue;

        m_component = component;

        qDebug()<< "Selecting component" << component;

        type = layout[BLUEPRINT_CONTROL_TYPE_INDEX + m_component];
        mode = configuration.preset[m_preset].component[m_component].bytes.mode;
        channel = configuration.preset[m_preset].component[m_component].bytes.channel;
        data = configuration.preset[m_preset].component[m_component].bytes.data;
        minValue = configuration.preset[m_preset].component[m_component].bytes.min;
        maxValue = configuration.preset[m_preset].component[m_component].bytes.max;

        setComponentChannel(channel);
        setComponentData(data);
        setComponentMinValue(minValue);
        setComponentMaxValue(maxValue);

        setComponentMode(static_cast<ComponentMode>(mode));
        setComponentButtonBehaviour(static_cast<ComponentButtonBehaviour>(type));

        emit componentChanged();

        return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     SELECT  & READ PRESET
/// \brief BackEnd::readPreset
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BackEnd::readPreset(const unsigned char &preset){

        this->timerLoop->stop();
        m_preset = preset;
        packet_num_buffer = 0;
        this ->timerRead->start(timerReadInterval);
        return;

}

void BackEnd::redPresetReq(const unsigned char &preset){

        this ->timerRead->stop();

        unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
        unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read

        memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);
        memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        data_out[0]=0; data_out[1]=SOF; data_out[2]=HOST_REQUIRES_READING; data_out[3]=CMD_PRESET; data_out[4]=preset;

        hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE);if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
                setDeviceStatus(Unplugged);
                conf_state = Request;
                qDebug() << "read error";
                this->timerLoop->start(timerLoopInterval);
                return;
        }

        qDebug() << "Read Preset request send" << preset;
        preset_status = WaitFinish_Preset;//preset_status = WaitOK_Preset;
        this->timerRead->start(timerReadInterval);
        return;

}

void BackEnd::WaitOKPreset(){

        this ->timerRead->stop();

        unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read

        memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        qDebug() << "#";
        if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
                setDeviceStatus(Unplugged);
                qDebug() << "read error";
                this->timerLoop->start(timerLoopInterval);
                return;
        }
        for (int i = 0; i < 64; ++i) {
                qDebug() << data_in[i];
        }
        if((data_in[0] != SOF) && (data_in[1] != OK) && (data_in[2] != CMD_PRESET)){
                qDebug() << "Ok";
                preset_status = WaitFinish_Preset;
        }

        this ->timerRead->start(timerReadInterval);
        return;

}

void BackEnd::WaitFinishPreset(const unsigned char &preset){

        this -> timerRead->stop();

        unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read

        memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        qDebug() << "Ok\n\nReading Data";

        if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
                setDeviceStatus(Unplugged);
                qDebug() << "read error";
                this->timerLoop->start(timerLoopInterval);
                return;
        }

        if(data_in[1] == FINISH){
                preset_status = Request_Preset;
                this->timerRead->stop();
                selectComponent(0);
                this->timerLoop->start(timerLoopInterval);
                return;
        }

        for (int i = 0; i < BLUEPRINT_USB_DATA_PACKET_SIZE; i++) {
                configuration.preset[preset].packet[packet_num_buffer].data[i] = data_in[i];
        }

        packet_num_buffer = packet_num_buffer + 1;
        this ->timerRead->start(timerReadInterval);
        return;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief BackEnd::syncHost2Device
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BackEnd::syncHost2Device(){
        bool temp_variable = true;
        setSynchronizing(temp_variable);
        this->timerLoop->stop();
        packet_num_buffer = 0;
        this ->timerSync->start(timerSyncInterval);
        return;
}

void BackEnd::syncReq(){
        qDebug() << "Sync";

        unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
        unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read

        memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);
        memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        data_out[0]=0; data_out[1]=SOF; data_out[2]=HOST_REQUIRES_WRITING; data_out[3]=CMD_LAYOUT; data_out[4]=0;

        hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE);

        if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
                qDebug() << " write error";
                setDeviceStatus(Unplugged);
                this->timerSync->stop();
                this->timerLoop->start(timerLoopInterval);
                return;
        }

        qDebug() << "Read request send";
        sync_status = SendLayout_Sync;
}

void BackEnd::WaitOKSync(){

        unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read
        memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
                setDeviceStatus(Unplugged);
                qDebug() << " read error";
                this->timerSync->stop();
                this->timerLoop->start(timerLoopInterval);
                return;
        }
        for (int i = 0; i < BLUEPRINT_USB_DATA_PACKET_SIZE; i++) {
                qDebug() << data_in[i + 1];
        }

        if((data_in[0] == SOF) && (data_in[1] == OK) ){
                qDebug() << "Ok";
                sync_status = SendLayout_Sync;
        }
        return;
}

void BackEnd::SendLayoutSync(){

        qDebug() << "Writting Data (Layout)  packet: " << packet_num_buffer;

        unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write

        //memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        int offset = BLUEPRINT_USB_DATA_PACKET_SIZE * packet_num_buffer;

        data_out[0] = 0;
        if(packet_num_buffer == 1)
                data_out[BLUEPRINT_USB_DATA_PACKET_SIZE] = 0xAA;
        for (unsigned char i = 0; i < BLUEPRINT_USB_DATA_PACKET_SIZE; i++){
                data_out[i + 1] = layout[offset + i];
        }

        if (hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
                qDebug() << " write error";
                setDeviceStatus(Unplugged);
                this->timerSync->stop();
                this->timerLoop->start(timerLoopInterval);
                return;
        }
        ++packet_num_buffer;
        QThread::msleep(100);
        if (packet_num_buffer > 1){
                packet_num_buffer = 0;
                sync_status = SendFinish_Sync;
        }
        return;
}

void BackEnd::SendFinishSync(){

        unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
        memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        data_out[0]=0; data_out[1]=SOF; data_out[2]=FINISH; data_out[3]=CMD_LAYOUT; data_out[4]=FINISH;

        if (hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
                qDebug() << " write error";
                setDeviceStatus(Unplugged);
                this->timerSync->stop();
                this->timerLoop->start(timerLoopInterval);
                return;
        }
        QThread::msleep(500);
        sync_status = SendReqPreset_Sync;
        return;
}

void BackEnd::SendReqPresetSync(){

        unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
        memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        data_out[0]=0; data_out[1]=SOF; data_out[2]=HOST_REQUIRES_WRITING; data_out[3]=CMD_CHANGES; data_out[4]=m_preset;

        if (hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
                qDebug() << " write error";
                setDeviceStatus(Unplugged);
                this->timerSync->stop();
                this->timerLoop->start(timerLoopInterval);
                return;
        }

        qDebug() << "Write Preset Request Send";

        QThread::msleep(500);
        sync_status = SendPreset_Sync;
        return;
}

void BackEnd::WaitOK1Sync(){

        unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read
        memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
                setDeviceStatus(Unplugged);
                qDebug() << " read error";
                this->timerSync->stop();
                this->timerLoop->start(timerLoopInterval);
                return;
        }

        if((data_in[0] == SOF) && (data_in[1] == OK) ){
                qDebug() << "Ok";
                sync_status = SendPreset_Sync;
        }
        return;
}

void BackEnd::SendPresetSync(){

        unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
        memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);
        qDebug() << "Sending preset" << m_preset << " package" << packet_num_buffer;

        int offset = BLUEPRINT_PRESET_DATA_SIZE * m_preset + BLUEPRINT_USB_DATA_PACKET_SIZE * packet_num_buffer;

        data_out[0] = 0;
        for(int i = 0; i < BLUEPRINT_USB_DATA_PACKET_SIZE; i++){
                data_out[i + 1] = configuration.preset[m_preset].packet[packet_num_buffer].data[i];
                qDebug() << offset + i << " === " << data_out[i + 1];
        }

        if(hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
                setDeviceStatus(Unplugged);
                qDebug() << " read error";
                this->timerSync->stop();
                this->timerLoop->start(timerLoopInterval);
                return;
        }
        ++packet_num_buffer;
        QThread::msleep(100);
        if(packet_num_buffer >= 4){
                sync_status = SendFinish1_Sync;
                packet_num_buffer = 0;
        }
        return;
}

void BackEnd::SendFinish1Sync(){

        unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
        memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);

        data_out[0]=0; data_out[1]=SOF; data_out[2]=FINISH; data_out[3]=CMD_CHANGES; data_out[4]=m_preset;

        if (hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
                qDebug() << " write error";
                setDeviceStatus(Unplugged);
                this->timerSync->stop();
                this->timerLoop->start(timerLoopInterval);
                return;
        }

        qDebug() << "Already sync!";
        QThread::msleep(500);
        setDeviceStatus(Working);

        sync_status = Request_Sync;
        bool temp_variable = false;
        setSynchronizing(temp_variable);
        this->timerSync->stop();
        this->timerLoop->start(timerLoopInterval);
        return;
}

///////////////////////////////////////////////////
/// \brief BackEnd::senseValue
//////////////////////////////////////////////////

void BackEnd::senseValue(){
        //return;
        this->timerLoop->stop();

        unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE];

        // GGG, changed timeout from 1000 to 0
        if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 0) < 0) {
                setDeviceStatus(Unplugged);
                qDebug() << "error\n";
                this->timerLoop->start(timerLoopInterval);
                return;
        }
        this->timerLoop->start(timerLoopInterval);
        return;
}

