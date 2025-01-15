#include "common.h"

double SWAPDOUBLE(double src)
{
    double des;
    char *s,*d;
    s=(char*)&src;
    d=(char*)&des;
    *(d+0)=*(s+7);
    *(d+1)=*(s+6);
    *(d+2)=*(s+5);
    *(d+3)=*(s+4);
    *(d+4)=*(s+3);
    *(d+5)=*(s+2);
    *(d+6)=*(s+1);
    *(d+7)=*(s+0);
    return des;
}

void MotorControl(t_Motor *m ,t_emc_control *mc)
{
    t_emc_control s_control;

    uint32_t valid=0xAABBCCDD;

    s_control.valid=htobe32(valid);
    s_control.start_block_cause=htobe32(mc->start_block_cause);
    s_control.unblock=htobe32(mc->unblock);
    s_control.drive_en=htobe32(mc->drive_en);

    s_control.test_mode=htobe32(mc->test_mode);
    s_control.mode=htobe32(mc->mode);

    s_control.target_speed=SWAPDOUBLE(mc->target_speed);
    s_control.target_torque=SWAPDOUBLE(mc->target_torque);
    //s_control.brk=SWAPDOUBLE(mc->brk);

    QUdpSocket().writeDatagram((char*)&s_control,sizeof(t_emc_control),m->ip,P_UDP_MIN_CONTROL_M+m->id);
}
