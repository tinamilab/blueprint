#ifndef DEFS_H
#define DEFS_H

/* Control Device Values */
#define SOF                                     0xA0
#define HOST_REQUIRES_READING                   0XA1
#define HOST_REQUIRES_WRITING                   0XA2
#define OK                                      0XA3
#define FINISH                                  0XA5

/*Comand values*/
#define CMD_LAYOUT                              0x01
#define CMD_PRESET                              0x02
#define CMD_CHANGES                             0x03

/*Control Values*/
#define LAYOUT_NUMBER                           0x00 /*0-15 LAYOUT NUMBER*/
#define COMPONENT_NUMBER                        0x01 /*0-15*/
#define CONTROL_TYPE                            0x03


/* Code Index Number (CIN) values */
/*   Table 4-1 of midi10.pdf      */
#define MIDI_CIN_MISC_FUNCTION_RESERVED         0x0
#define MIDI_CIN_CABLE_EVENTS_RESERVED          0x1
#define MIDI_CIN_2_uint8_t_MESSAGE              0x2
#define MIDI_CIN_MTC                            0x2
#define MIDI_CIN_SONG_SELECT                    0x2
#define MIDI_CIN_3_uint8_t_MESSAGE              0x3
#define MIDI_CIN_SSP                            0x3
#define MIDI_CIN_SYSEX_START                    0x4
#define MIDI_CIN_SYSEX_CONTINUE                 0x4
#define MIDI_CIN_1_uint8_t_MESSAGE              0x5
#define MIDI_CIN_SYSEX_ENDS_1                   0x5
#define MIDI_CIN_SYSEX_ENDS_2                   0x6
#define MIDI_CIN_SYSEX_ENDS_3                   0x7
#define MIDI_CIN_NOTE_OFF                       0x8
#define MIDI_CIN_NOTE_ON                        0x9
#define MIDI_CIN_POLY_KEY_PRESS                 0xA
#define MIDI_CIN_CONTROL_CHANGE                 0xB
#define MIDI_CIN_PROGRAM_CHANGE                 0xC
#define MIDI_CIN_CHANNEL_PREASURE               0xD
#define MIDI_CIN_PITCH_BEND_CHANGE              0xE
#define MIDI_CIN_SINGLE_uint8_t                 0xF

/*
*/
enum controlType_t
{
    tNone,
    tButton_pulse,
    tPot
};

enum property_t
{
    property_mode,
    property_ccNumber,
    property_channel,
    property_minValue,
    property_maxValue,

};

#endif // DEFS_H
