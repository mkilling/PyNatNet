
#include <winsock2.h>   // must include before windows.h or ws2tcpip.h
#include <windows.h>
#include <Ws2tcpip.h>
#include <tchar.h>
#include <math.h>
#include "NATUtils.h"
#include <float.h>

#pragma warning( disable : 4244 )

int NATUtils::GetLocalIPAddresses(unsigned long Addresses[], int nMax)
{
    //InitializeWinsock();
    // init windows sockets lib
    static WSADATA WsaData;
    if (WSAStartup(0x202, &WsaData) == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        WSACleanup();
        return error;
    }

    struct hostent *haddr;
    char           szMyName[128];
    unsigned long  NameLength = 128;
    int            nAddresses;

    GetComputerName(szMyName, &NameLength);
    if (!(haddr=gethostbyname(szMyName)))
    {
        return -1;
    }

    for (nAddresses=0 ; nAddresses<nMax ; nAddresses++)
    {
        if (haddr->h_addr_list[nAddresses] == NULL)
        {
            break;
        }

        memcpy(&Addresses[nAddresses], haddr->h_addr_list[nAddresses], 4);
    }

    return nAddresses;
}

void NATUtils::quatToMatrix(double *q, double *m)
{
    m[0] = 1-2*q[1]*q[1]-2*q[2]*q[2]; m[1] = 2*q[0]*q[1]-2*q[3]*q[2];   m[2] = 2*q[0]*q[2]+2*q[3]*q[1];
    m[3] = 2*q[0]*q[1]+2*q[3]*q[2];   m[4] = 1-2*q[0]*q[0]-2*q[2]*q[2]; m[5] = 2*q[1]*q[2]-2*q[3]*q[0];
    m[6] = 2*q[0]*q[2]-2*q[3]*q[1];   m[7] = 2*q[1]*q[2]+2*q[3]*q[0];   m[8] = 1-2*q[0]*q[0]-2*q[1]*q[1];
}

void NATUtils::vec3MatrixMult(double *v, double *m)
{
    float x = v[0]*m[0]+v[1]*m[3]+v[2]*m[6];
    float y = v[0]*m[1]+v[1]*m[4]+v[2]*m[7];
    float z = v[0]*m[2]+v[1]*m[5]+v[2]*m[8];
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

void NATUtils::Matrix3AndPosToMatrix4(double* m3, double* v, double* m4)
{
    m4[0] = m3[0];
    m4[1] = m3[1];
    m4[2] = m3[2];
    m4[3] = 0.0f;

    m4[4] = m3[3];
    m4[5] = m3[4];
    m4[6] = m3[5];
    m4[7] = 0.0f;

    m4[8] = m3[6];
    m4[9] = m3[7];
    m4[10] = m3[8];
    m4[11] = 0.0f;

    m4[12] = v[0];
    m4[13] = v[1];
    m4[14] = v[2];
    m4[15] = 1.0f;
}

EulerAngles Eul_(float ai, float aj, float ah, int order)
{
    EulerAngles ea;
    ea.x = ai; ea.y = aj; ea.z = ah;
    ea.w = order;
    return (ea);
}

/* Construct quaternion from Euler angles (in radians). */
Quat Eul_ToQuat(EulerAngles ea)
{
    Quat qu;
    double a[3], ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int i,j,k,h,n,s,f;
    EulGetOrd(ea.w,i,j,k,h,n,s,f);
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}
    if (n==EulParOdd) ea.y = -ea.y;
    ti = ea.x*0.5; tj = ea.y*0.5; th = ea.z*0.5;
    ci = cos(ti);  cj = cos(tj);  ch = cos(th);
    si = sin(ti);  sj = sin(tj);  sh = sin(th);
    cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
    if (s==EulRepYes) {
        a[i] = cj*(cs + sc);	/* Could speed up with */
        a[j] = sj*(cc + ss);	/* trig identities. */
        a[k] = sj*(cs - sc);
        qu.w = cj*(cc - ss);
    } else {
        a[i] = cj*sc - sj*cs;
        a[j] = cj*ss + sj*cc;
        a[k] = cj*cs - sj*sc;
        qu.w = cj*cc + sj*ss;
    }
    if (n==EulParOdd) a[j] = -a[j];
    qu.x = a[X]; qu.y = a[Y]; qu.z = a[Z];
    return (qu);
}

/* Construct matrix from Euler angles (in radians). */
void Eul_ToHMatrix(EulerAngles ea, HMatrix M)
{
    double ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int i,j,k,h,n,s,f;
    EulGetOrd(ea.w,i,j,k,h,n,s,f);
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}
    if (n==EulParOdd) {ea.x = -ea.x; ea.y = -ea.y; ea.z = -ea.z;}
    ti = ea.x;	  tj = ea.y;	th = ea.z;
    ci = cos(ti); cj = cos(tj); ch = cos(th);
    si = sin(ti); sj = sin(tj); sh = sin(th);
    cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
    if (s==EulRepYes) {
        M[i][i] = cj;	  M[i][j] =  sj*si;    M[i][k] =  sj*ci;
        M[j][i] = sj*sh;  M[j][j] = -cj*ss+cc; M[j][k] = -cj*cs-sc;
        M[k][i] = -sj*ch; M[k][j] =  cj*sc+cs; M[k][k] =  cj*cc-ss;
    } else {
        M[i][i] = cj*ch; M[i][j] = sj*sc-cs; M[i][k] = sj*cc+ss;
        M[j][i] = cj*sh; M[j][j] = sj*ss+cc; M[j][k] = sj*cs-sc;
        M[k][i] = -sj;	 M[k][j] = cj*si;    M[k][k] = cj*ci;
    }
    M[W][X]=M[W][Y]=M[W][Z]=M[X][W]=M[Y][W]=M[Z][W]=0.0; M[W][W]=1.0;
}

/* Convert matrix to Euler angles (in radians). */
EulerAngles Eul_FromHMatrix(HMatrix M, int order)
{
    EulerAngles ea;
    int i,j,k,h,n,s,f;
    EulGetOrd(order,i,j,k,h,n,s,f);
    if (s==EulRepYes) {
        double sy = sqrt(M[i][j]*M[i][j] + M[i][k]*M[i][k]);
        if (sy > 16*FLT_EPSILON) {
            ea.x = atan2((double)M[i][j], (double)M[i][k]);
            ea.y = atan2(sy, (double)M[i][i]);
            ea.z = atan2(M[j][i], -M[k][i]);
        } else {
            ea.x = atan2(-M[j][k], M[j][j]);
            ea.y = atan2(sy, (double)M[i][i]);
            ea.z = 0;
        }
    } else {
        double cy = sqrt(M[i][i]*M[i][i] + M[j][i]*M[j][i]);
        if (cy > 16*FLT_EPSILON) {
            ea.x = atan2(M[k][j], M[k][k]);
            ea.y = atan2((double)-M[k][i], cy);
            ea.z = atan2(M[j][i], M[i][i]);
        } else {
            ea.x = atan2(-M[j][k], M[j][j]);
            ea.y = atan2((double)-M[k][i], cy);
            ea.z = 0;
        }
    }
    if (n==EulParOdd) {ea.x = -ea.x; ea.y = - ea.y; ea.z = -ea.z;}
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}
    ea.w = order;
    return (ea);
}

/* Convert quaternion to Euler angles (in radians). */
EulerAngles Eul_FromQuat(Quat q, int order)
{
    HMatrix M;
    double Nq = q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
    double s = (Nq > 0.0) ? (2.0 / Nq) : 0.0;
    double xs = q.x*s,	  ys = q.y*s,	 zs = q.z*s;
    double wx = q.w*xs,	  wy = q.w*ys,	 wz = q.w*zs;
    double xx = q.x*xs,	  xy = q.x*ys,	 xz = q.x*zs;
    double yy = q.y*ys,	  yz = q.y*zs,	 zz = q.z*zs;
    M[X][X] = 1.0 - (yy + zz); M[X][Y] = xy - wz; M[X][Z] = xz + wy;
    M[Y][X] = xy + wz; M[Y][Y] = 1.0 - (xx + zz); M[Y][Z] = yz - wx;
    M[Z][X] = xz - wy; M[Z][Y] = yz + wx; M[Z][Z] = 1.0 - (xx + yy);
    M[W][X]=M[W][Y]=M[W][Z]=M[X][W]=M[Y][W]=M[Z][W]=0.0; M[W][W]=1.0;
    return (Eul_FromHMatrix(M, order));
}
