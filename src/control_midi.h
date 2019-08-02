#ifndef CONTROL_MIDI_H
#define CONTROL_MIDI_H

#include <QObject>
#include <QtGui>
#include <QObject>
#include <QList>
#include <QVariant>
#include <QVariantList>


class Control_midi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(unsigned char index READ index WRITE set_index NOTIFY indexChanged)
    Q_PROPERTY(unsigned char preset READ preset WRITE set_preset NOTIFY presetChanged)
    Q_PROPERTY(unsigned char componentType READ componentType WRITE set_componentType NOTIFY componentTypeChanged)
    Q_PROPERTY(QString mode READ mode WRITE set_mode NOTIFY modeChanged)
    Q_PROPERTY(unsigned char ccNumber READ ccNumber WRITE set_ccNumber NOTIFY ccNumberChanged)
    Q_PROPERTY(unsigned char minValue READ minValue WRITE set_minValue NOTIFY minValueChanged)
    Q_PROPERTY(unsigned char maxValue READ maxValue WRITE set_maxValue NOTIFY maxValueChanged)

public:
    explicit Control_midi(QObject *parent = nullptr);

    ~Control_midi();

    unsigned char index() const {return m_index;}
    unsigned char preset() const {return m_preset;}
    unsigned char componentType() const {return m_componentType;}
    QString mode() const {return m_mode;}
    unsigned char ccNumber() const {return m_ccNumber;}
    unsigned char minValue() const {return m_minValue;}
    unsigned char maxValue() const {return m_maxValue;}

public slots:
    void set_index(const unsigned char &index);
    void set_preset(const unsigned char &preset);
    void set_componentType(const unsigned char &componentType);
    void set_mode(const QString &mode);
    void set_ccNumber(const unsigned char &ccNumber);
    void set_minValue(const unsigned char &minValue);
    void set_maxValue(const unsigned char &maxValue);

signals:
    void indexChanged();
    void presetChanged();
    void componentTypeChanged();
    void modeChanged();
    void ccNumberChanged();
    void minValueChanged();
    void maxValueChanged();

private:
    unsigned char m_index;
    unsigned char m_preset;
    unsigned char m_componentType;
    QString m_mode;
    unsigned char m_ccNumber;
    unsigned char m_minValue;
    unsigned char m_maxValue;
};

#endif // CONTROL_MIDI_H
