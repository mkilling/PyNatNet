#include <Python.h>
#include <cstdio>
#include <NatNetTypes.h>
#include <NatNetClient.h>

void DataHandler(sFrameOfMocapData* data, void* pUserData) {
    PyObject *callback = (PyObject *)pUserData;
    PyObject *arglist = Py_BuildValue("(O)", 
    PyEval_CallObject(callback, arglist);
}

PyObject *cnatnet_constructor(PyObject *self, PyObject *args) {
    int version;
    if (!PyArg_ParseTuple(args, "i", &version))
        return NULL;
    NatNetClient *inst = new NatNetClient(version);    
    return PyCObject_FromVoidPtr(inst, NULL);
}

PyObject *cnatnet_initialize(PyObject *self, PyObject *args) {
    PyObject *pyInst;
    char *myIpAddress, *serverIpAddress;
    PyArg_ParseTuple(args, "Oss", &pyInst, &myIpAddress, &serverIpAddress);
    NatNetClient *inst = (NatNetClient *)PyCObject_AsVoidPtr(pyInst);
    int ret = inst->Initialize(myIpAddress, serverIpAddress);
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
    Py_XINCREF(callback);
    NatNetClient *inst = (NatNetClient *)PyCObject_AsVoidPtr(pyInst);
    inst->SetDataCallback(DataHandler, callback);
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
}
