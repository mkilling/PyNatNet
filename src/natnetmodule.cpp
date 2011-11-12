#include <Python.h>
#include <cstdio>
#include <NatNetTypes.h>
#include <NatNetClient.h>

PyObject *FrameOfMocapData;
PyObject *RigidBodyData;
PyObject *MarkerSetDescription;
PyObject *RigidBodyDescription;
PyObject *SkeletonDescription;

void DataHandler(sFrameOfMocapData *data, void *pUserData) {
    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject *callback = (PyObject *)pUserData;
    PyObject *rigidBodies = PyList_New(0);
    for (int i = 0; i < data->nRigidBodies; i++) {
        sRigidBodyData body = data->RigidBodies[i];
        PyObject *rigidBodyArgs = Py_BuildValue("(ifffffff)", body.ID, body.x, body.y, body.z, body.qx, body.qy, body.qz, body.qw);
        PyObject *dict = PyDict_New();
        PyObject *rigidBody = PyInstance_New(RigidBodyData, rigidBodyArgs, dict);
        Py_DECREF(dict);
        Py_DECREF(rigidBodyArgs);
        PyList_Append(rigidBodies, rigidBody);
        Py_DECREF(rigidBody);
    }

    PyObject *mocapDataArgs = Py_BuildValue("(iOf)", data->iFrame, rigidBodies, data->fLatency);
    PyObject *dict = PyDict_New();
    PyObject *mocapInst = PyInstance_New(FrameOfMocapData, mocapDataArgs, dict);
    Py_DECREF(dict);
    PyObject *arglist = Py_BuildValue("(O)", mocapInst);
    PyEval_CallObject(callback, arglist);
    Py_DECREF(arglist);
    Py_DECREF(mocapInst);
    Py_DECREF(mocapDataArgs);
    Py_DECREF(rigidBodies);

    PyGILState_Release(gstate);
}

PyObject *cnatnet_constructor(PyObject *self, PyObject *args) {
    int iType;
    if (!PyArg_ParseTuple(args, "i", &iType))
        return NULL;
    NatNetClient *inst = new NatNetClient(iType);
    PyObject *ret = PyCObject_FromVoidPtr(inst, NULL);
    return ret;
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
    int ret;
    Py_BEGIN_ALLOW_THREADS
    ret = inst->Initialize(szMyIPAddress, szServerIPAddress);
    Py_END_ALLOW_THREADS
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

PyObject *cnatnet_getDataDescriptions(PyObject *self, PyObject *args) {
    PyObject *pyInst;
    PyArg_ParseTuple(args, "O", &pyInst);
    NatNetClient *inst = (NatNetClient *)PyCObject_AsVoidPtr(pyInst);
    sDataDescriptions *dataDescriptions;
    inst->GetDataDescriptions(&dataDescriptions);
    PyObject *retList = PyList_New(0);
    for (int i = 0; i < dataDescriptions->nDataDescriptions; i++) {
        sDataDescription *dataDescription = &dataDescriptions->arrDataDescriptions[i];
        PyObject *descObj;
        PyObject *args;
        PyObject *dict = PyDict_New();
        switch (dataDescription->type) {
            case Descriptor_RigidBody: {
                sRigidBodyDescription *rigidBodyDesc = dataDescription->Data.RigidBodyDescription;
                args = Py_BuildValue("(siifff)", rigidBodyDesc->szName, rigidBodyDesc->ID, rigidBodyDesc->parentID, rigidBodyDesc->offsetx, rigidBodyDesc->offsety, rigidBodyDesc->offsetz);
                descObj = PyInstance_New(RigidBodyDescription, args, dict);
                }
                break;
            case Descriptor_MarkerSet:
            case Descriptor_Skeleton:
                // not yet supported
                continue;
        }
        Py_DECREF(dict);
        Py_DECREF(args);
        PyList_Append(retList, descObj);
    }
    return retList;
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
    {"getDataDescriptions", cnatnet_getDataDescriptions, METH_VARARGS, "NatNetClient::GetDataDescriptions"},
    {"setMessageCallback", cnatnet_setMessageCallback, METH_VARARGS, "NatNetClient::SetMessageCallback"},
    {"setVerbosityLevel", cnatnet_setVerbosityLevel, METH_VARARGS, "NatNetClient::SetVerbosityLevel"},
    {"setDataCallback", cnatnet_setDataCallback, METH_VARARGS, "NatNetClient::SetDataCallback"},
    {NULL, NULL, 0, NULL}    /* Sentinel */
};

PyMODINIT_FUNC initcnatnet() {
    Py_InitModule("cnatnet", cnatnet_funcs);
    PyEval_InitThreads();
    FrameOfMocapData = PyObject_GetAttrString(PyImport_ImportModule("NatNet.FrameOfMocapData"), "FrameOfMocapData");
    Py_INCREF(FrameOfMocapData);
    RigidBodyData = PyObject_GetAttrString(PyImport_ImportModule("NatNet.RigidBodyData"), "RigidBodyData");
    Py_INCREF(RigidBodyData);

    PyObject *descriptionModule = PyImport_ImportModule("NatNet.DataDescription");
    MarkerSetDescription = PyObject_GetAttrString(descriptionModule, "MarkerSetDescription");
    Py_INCREF(MarkerSetDescription);
    RigidBodyDescription = PyObject_GetAttrString(descriptionModule, "RigidBodyDescription");
    Py_INCREF(RigidBodyDescription);
    SkeletonDescription = PyObject_GetAttrString(descriptionModule, "SkeletonDescription");
    Py_INCREF(SkeletonDescription);
}
