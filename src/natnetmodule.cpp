#include <Python.h>
#include <cstdio>
#include <NatNetTypes.h>
#include <NatNetClient.h>

PyObject *cnatnet_constructor(PyObject *self, PyObject *args) {
	int version;
	if (!PyArg_ParseTuple(args, "i", &version))
        return NULL;
	NatNetClient *inst = new NatNetClient(version);	
	return PyCObject_FromVoidPtr(inst, NULL);
}

PyObject *cnatnet_natNetVersion(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	PyArg_ParseTuple(args, "O", &pyInst);
	NatNetClient *inst = (NatNetClient *)PyCObject_AsVoidPtr(pyInst);
	unsigned char ver[4];
	inst->NatNetVersion(ver);
	return PyTuple_Pack(4, PyInt_FromSize_t(ver[0]), PyInt_FromSize_t(ver[1]), PyInt_FromSize_t(ver[2]), PyInt_FromSize_t(ver[3]));
}

PyObject *cnatnet_setMessageCallback(PyObject *self, PyObject *args) {
	Py_RETURN_NONE;
}

PyMethodDef cnatnet_funcs[] = {
	{"constructor", cnatnet_constructor, METH_VARARGS, "NatNetClient::NatNetClient"},
	{"natNetVersion", cnatnet_natNetVersion, METH_VARARGS, "NatNetClient::NatNetVersion"},
	{"setMessageCallback", cnatnet_setMessageCallback, METH_VARARGS, "NatNetClient::SetMessageCallback"},
	{NULL, NULL, 0, NULL}	/* Sentinel */
};

PyMODINIT_FUNC initcnatnet(void) {
	(void)Py_InitModule("cnatnet", cnatnet_funcs);
}
