#ifndef TINAMICOMM_H
#define TINAMICOMM_H

#include <QObject>

class TinamiComm : public QObject
{
        Q_OBJECT
public:
        explicit TinamiComm(QObject *parent = nullptr);

signals:

public slots:
};

#endif // TINAMICOMM_H