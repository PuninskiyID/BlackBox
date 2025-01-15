#ifndef _CAN_GW_H_
#define _CAN_GW_H_

#include <QObject>
#include <QUdpSocket>

#define GW_TYPE_1_NUCLEO_H23ZG_ETH 1
#define GW_TYPE_4_TMS570_ETH 4

//<. EXT><.. BUS[0;3]><.* ID>
#define CAN_GW_CAN_MES(_ext, _id, _bus) ( ( (_ext)? 0x80000000ULL : 0ULL ) | ( ((uint64_t)(_bus) & 0x3ULL) << 29 ) | ( (_id) & ((_ext)? 0x1FFFFFFFULL : 0x7FFULL) ) )
#define CAN_GW_CAN_MES_BUS_Pos  29
#define CAN_GW_CAN_MES_BUS      (0x3ULL << CAN_GW_CAN_MES_BUS_Pos)
#define CAN_GW_CAN_MES_BUS_MAX  (CAN_GW_CAN_MES_BUS >> CAN_GW_CAN_MES_BUS_Pos)
#define CAN_GW_CAN_MES_EXT_Pos  31
#define CAN_GW_CAN_MES_EXT      (0x1ULL << CAN_GW_CAN_MES_EXT_Pos)
#define CAN_GW_CAN_MES_ID       0x1FFFFFFFULL
class CanGateway: public QObject
{
    Q_OBJECT
protected:
    bool enCanMes = 0, enDbg1 = 0, enCon1 = 0, enCon2 = 0, enCon3 = 0;
public:
    enum Channel: uint64_t { CAN_MES = 0xFFFFFFFFULL, DBG1 = (1ULL << 32), CON1 = (1ULL << 33), CON2 = (1ULL << 34), CON3 = (1ULL << 35) };
    CanGateway(): QObject(0) { }
    virtual void send(uint64_t channel, const char* data, int size) = 0;
    void setChannelEnable(uint64_t channel, bool en)
    {
        if(channel & CAN_MES) { enCanMes = en; }
        if(channel & DBG1) { enDbg1 = en; }
        if(channel & CON1) { enCon1 = en; }
        if(channel & CON2) { enCon2 = en; }
        if(channel & CON3) { enCon3 = en; }
    }
signals:
    void rec(uint64_t channel, const char* data, int size);
};

class CanGateway_NucleoH723ZG_Eth: public CanGateway
{
    Q_OBJECT
    QUdpSocket *sock;
    QHostAddress addr; //7.7.7.204
    static const quint16 portEmcCon = 49001;
    static const quint16 portEmcDbg = 49002;
    static const quint16 portDcuCon = 49055;
    static const quint16 portPmuCon = 49044;
public:
    ~CanGateway_NucleoH723ZG_Eth() { delete sock; }
    CanGateway_NucleoH723ZG_Eth(): CanGateway(), addr("7.7.7.204")
    {
        sock = new QUdpSocket();
        sock->bind(QHostAddress::AnyIPv4);
        connect(sock, SIGNAL(readyRead()), this, SLOT(udpRec()));
    }
    void setAddress(const QHostAddress& addr)
    {
        this->addr = addr;
    }
    void send(uint64_t channel, const char* data, int size) override
    {
        if(channel & CAN_MES)
        {
            return;
        }
        switch(channel)
        {
        case DBG1: if(enCon1) sock->writeDatagram(data, size, addr, portEmcDbg); break;
        case CON1: if(enDbg1) sock->writeDatagram(data, strlen(data) + 1, addr, portEmcCon); break;
        case CON2: if(enCon2) sock->writeDatagram(data, strlen(data) + 1, addr, portDcuCon); break;
        case CON3: if(enCon3) sock->writeDatagram(data, strlen(data) + 1, addr, portPmuCon); break;
        }
    }
private slots:
    void udpRec()
    {
        QHostAddress host;
        quint16 port;
        int size;
        char data[1024];
        while(sock->hasPendingDatagrams())
        {
            size = sock->readDatagram(data, sizeof(data), &host, &port);
            if(size <= 0) continue;
            switch(port)
            {
            case portEmcCon: if(enCon1) emit rec(CON1, data, size); break;
            case portEmcDbg: if(enDbg1) emit rec(DBG1, data, size); break;
            case portDcuCon: if(enCon2) emit rec(CON2, data, size); break;
            case portPmuCon: if(enCon3) emit rec(CON3, data, size); break;
            }

        }
    }
};

class CanGateway_Tms570: public CanGateway
{
    typedef struct
    {
        uint32_t node;
        uint32_t eid;
        uint32_t id;
        uint32_t dlc;
        uint8_t data[8];
    } t_can_gw;

    Q_OBJECT
    QUdpSocket *sock;
    QHostAddress addr; //7.7.7.200
    quint16 port; //21000..21003
    char fragBuf[1024];
    int fragOff = 0;
public:
    ~CanGateway_Tms570()
    {
        delete sock;
    }
    CanGateway_Tms570(QHostAddress addr, quint16 port): CanGateway(), addr(addr)
    {
        if(port < 21000)
        {
            port = 21000;
            qDebug() << "CanGateway_Ver4_Tms570 Port < 21000";
        }
        else if(port > 21003)
        {
            port = 21003;
            qDebug() << "CanGateway_Ver4_Tms570 Port < 21000";
        }
        this->port = port;
        sock = new QUdpSocket();
        sock->bind(QHostAddress::AnyIPv4);
        connect(sock, SIGNAL(readyRead()), this, SLOT(udpRec()));
    }
    void send(uint64_t channel, const char* data, int size) override
    {
        t_can_gw m;

        if(channel & CAN_MES)
        {
            if(enCanMes)
            {
                m.eid = (channel & CAN_GW_CAN_MES_EXT);
                m.node = (channel & CAN_GW_CAN_MES_BUS) >> CAN_GW_CAN_MES_BUS_Pos;
                m.id = channel & CAN_GW_CAN_MES_ID;
                m.dlc = size % 9;
                memcpy(m.data, data, size % 9);
                sock->writeDatagram((char*)&m, sizeof(m), addr, port);
            }
            return;
        }

        if(channel & DBG1)
        {
            if(enDbg1)
            {
                m.eid = 0;
                m.node = 0;
                m.id = 0x80;
                m.dlc = 8;

                while(size > 8)
                {
                    memcpy(m.data, data, 8);
                    sock->writeDatagram((char*)&m, sizeof(m), addr, port);
                    size -= 8;
                    data += 8;
                }

                m.id = 0x81;
                m.dlc = size;
                memcpy(m.data, data, size);
                sock->writeDatagram((char*)&m, sizeof(m), addr, port);
            }
            return;
        }
    }
private slots:
    void udpRec()
    {
        QHostAddress host;
        quint16 port;
        char buf[sizeof(t_can_gw)];

        while( sock->hasPendingDatagrams() )
        {
            int size = sock->readDatagram(buf, sizeof(buf), &host, &port);

            if(size != sizeof(t_can_gw)) continue;
            auto m = (t_can_gw*)buf;

            if(enCanMes && m->node <= CAN_GW_CAN_MES_BUS_MAX)
            {
                emit rec(CAN_GW_CAN_MES(m->eid, m->node, m->id), (const char*)m->data, (int)(uint32_t)m->dlc);
            }

            if(enDbg1 && m->node == 0 && m->eid == 0 && (m->id == 0x82 || m->id == 0x83))
            {
                memcpy(fragBuf + fragOff, m->data, m->dlc);
                if(m->id == 0x82)
                {
                    fragOff += m->dlc;
                }
                else
                {
                    emit rec(DBG1, fragBuf, fragOff + m->dlc);
                    fragOff = 0;
                }
            }
        }
    }
};

#endif /*_CAN_GW_H_*/
