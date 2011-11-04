#include <Python.h>
#include <cstdio>
#include <NatNetTypes.h>
#include <NatNetClient.h>

PyObject *cnatnet_test_func(PyObject *self, PyObject *args) {
	Py_RETURN_TRUE;
}

PyObject *cnatnet_constructor(PyObject *self, PyObject *args) {
	int version;
	if (!PyArg_ParseTuple(args, "i", &version))
        return NULL;
	NatNetClient *inst = new NatNetClient(version);	
	return PyCObject_FromVoidPtr(inst, NULL);
}

PyMethodDef cnatnet_funcs[] = {
	{"test_func", cnatnet_test_func, METH_VARARGS, "test function"},
	{"constructor", cnatnet_constructor, METH_VARARGS, "NatNetClient constructor"},
	{NULL, NULL, 0, NULL}	/* Sentinel */
};

PyMODINIT_FUNC initcnatnet(void) {
	(void)Py_InitModule("cnatnet", cnatnet_funcs);
}
