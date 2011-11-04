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
	return PyString_FromFormat("%d.%d.%d.%d", ver[0], ver[1], ver[2], ver[3]);
}

PyMethodDef cnatnet_funcs[] = {
	{"constructor", cnatnet_constructor, METH_VARARGS, "NatNetClient::NatNetClient"},
	{"natNetVersion", cnatnet_natNetVersion, METH_VARARGS, "NatNetClient::NatNetVersion"},
	{NULL, NULL, 0, NULL}	/* Sentinel */
};

PyMODINIT_FUNC initcnatnet(void) {
	(void)Py_InitModule("cnatnet", cnatnet_funcs);
}
