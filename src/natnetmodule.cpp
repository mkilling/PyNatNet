#include <Python.h>
#include <cstdio>
#include <NatNetTypes.h>
#include <NatNetClient.h>

PyObject *FrameOfMocapData;
PyObject *RigidBodyData;

void DataHandler(sFrameOfMocapData* data, void* pUserData) {
    printf("Callback in C++\n");
    PyObject *callback = (PyObject *)pUserData;
    
    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject *rigidBodies = PyList_New(0);
    for (int i = 0; i < data->nRigidBodies; i++) {
        sRigidBodyData body = data->RigidBodies[i];
        PyObject *rigidBodyArgs = Py_BuildValue("(ifffffff)", body.ID, body.x, body.y, body.z, body.qx, body.qy, body.qz, body.qw);
        PyObject *rigidBody = PyInstance_New(RigidBodyData, rigidBodyArgs, PyDict_New());
        PY_DECREF(rigidBodyArgs);
        PyList_Append(rigidBodies, rigidBody);
        Py_DECREF(rigidBody);
    }
    
    PyObject *mocapDataArgs = Py_BuildValue("(iO)", data->iFrame, rigidBodies); 
    PyObject *mocapInst = PyInstance_New(FrameOfMocapData, mocapDataArgs, PyDict_New());    
    PyObject *arglist = Py_BuildValue("(O)", mocapInst); 
    PyEval_CallObject(callback, arglist);
    Py_DECREF(arglist);
    Py_DECREF(mocapInst);
    Py_DECREF(mocapDataArgs);
    Py_DECREF(rgidBodies);
    
    PyGILState_Release(gstate);
}

PyObject *cnatnet_constructor(PyObject *self, PyObject *args) {
    int iType;
    if (!PyArg_ParseTuple(args, "i", &iType))
        return NULL;
    NatNetClient *inst = new NatNetClient(iType);    
    return PyCObject_FromVoidPtr(inst, NULL);
}

PyObject *cnatnet_initialize(PyObject *self, PyObject *args) {
    PyObject *pyInst;
    char *myIpAddress, *serverIpAddress;
    PyArg_ParseTuple(args, "Oss", &pyInst, &myIpAddress, &serverIpAddress);
    NatNetClient *inst = (NatNetClient *)PyCObject_AsVoidPtr(pyInst);
    char szMyIPAddress[128] = {0};
    char szServerIPAddress[128] = {0};
    strncpy(szMyIPAddress, myIpAddress, 127);
    strncpy(szServerIPAddress, serverIpAddress, 127);
    int ret = inst->Initialize(szMyIPAddress, szServerIPAddress);
    return PyInt_FromLong(ret);
}

PyObject *cnatnet_natNetVersion(PyObject *self, PyObject *args) {
    PyObject *pyInst;
    PyArg_ParseTuple(args, "O", &pyInst);
    NatNetClient *inst = (NatNetClient *)PyCObject_AsVoidPtr(pyInst);
    unsigned char ver[4];
    inst->NatNetVersion(ver);
    return PyTuple_Pack(4, PyInt_FromSize_t(ver[0]), PyInt_FromSize_t(ver[1]), PyInt_FromSize_t(ver[2]), PyInt_FromSize_t(ver[3]));
}

PyObject *cnatnet_getServerDescription(PyObject *self, PyObject *args) {
    Py_RETURN_NONE;
}

PyObject *cnatnet_setMessageCallback(PyObject *self, PyObject *args) {
    Py_RETURN_NONE;
}

PyObject *cnatnet_setVerbosityLevel(PyObject *self, PyObject *args) {
    PyObject *pyInst;
    int level;
    PyArg_ParseTuple(args, "Oi", &pyInst, &level);
    NatNetClient *inst = (NatNetClient *)PyCObject_AsVoidPtr(pyInst);
    inst->SetVerbosityLevel(level);
    Py_RETURN_NONE;
}

PyObject *cnatnet_setDataCallback(PyObject *self, PyObject *args) {
    PyObject *pyInst;
    PyObject *callback;
    PyArg_ParseTuple(args, "OO", &pyInst, &callback);
    Py_INCREF(callback);
    NatNetClient *inst = (NatNetClient *)PyCObject_AsVoidPtr(pyInst);
    inst->SetDataCallback(DataHandler, callback);
    Py_RETURN_NONE;
}

PyMethodDef cnatnet_funcs[] = {
    {"constructor", cnatnet_constructor, METH_VARARGS, "NatNetClient::NatNetClient"},
    {"initialize", cnatnet_initialize, METH_VARARGS, "NatNetClient::Initialize"},
    {"natNetVersion", cnatnet_natNetVersion, METH_VARARGS, "NatNetClient::NatNetVersion"},
    {"getServerDescription", cnatnet_getServerDescription, METH_VARARGS, "NatNetClient::GetServerDescription"},
    {"setMessageCallback", cnatnet_setMessageCallback, METH_VARARGS, "NatNetClient::SetMessageCallback"},
    {"setVerbosityLevel", cnatnet_setVerbosityLevel, METH_VARARGS, "NatNetClient::SetVerbosityLevel"},
    {"setDataCallback", cnatnet_setDataCallback, METH_VARARGS, "NatNetClient::SetDataCallback"},
    {NULL, NULL, 0, NULL}    /* Sentinel */
};

PyMODINIT_FUNC initcnatnet(void) {
    (void)Py_InitModule("cnatnet", cnatnet_funcs);
    FrameOfMocapData = PyObject_GetAttrString(PyImport_ImportModule("NatNet.FrameOfMocapData"), "FrameOfMocapData");
    Py_INCREF(FrameOfMocapData);
    RigidBodyData = PyObject_GetAttrString(PyImport_ImportModule("NatNet.RigidBodyData"), "RigidBodyData");
    Py_INCREF(RigidBodyData);
}
