#include <Python.h>
#include <stdio.h>

static int HR[32487834];

static inline int LookupHandFast(int* pCards) {
	// manually unrolled loop
	int p = HR[53 + *pCards++];
	p = HR[p + *pCards++];
	p = HR[p + *pCards++];
	p = HR[p + *pCards++];
	p = HR[p + *pCards++];
	p = HR[p + *pCards++];
	return HR[p + *pCards++];
}

static inline int LookupHand(int* pCards) {
	int p = 53;
	int i = 0;
	do {
		p = HR[p + *pCards];
	} while (*pCards++ && ++i < 7);
	return p;
}

static inline void convert(PyObject *cards_seq, int len, int *outRet) {
	int i = 0;
	for (; i < len; i++)
		outRet[i] = PyInt_AsLong(PySequence_GetItem(cards_seq, i));
}

static PyObject *crank_get_rank(PyObject *self, PyObject *args) {
	PyObject *pyseq;
	if (!PyArg_ParseTuple(args, "O", &pyseq))
		return NULL;
	int hand[7] = {0};
	convert(pyseq, PySequence_Length(pyseq), hand);
	return Py_BuildValue("i", LookupHand(hand));
}

static PyObject *crank_load_table(PyObject *self, PyObject *args) {
	const char *path;
	if (!PyArg_ParseTuple(args, "s", &path))
		return NULL;
	
	FILE *f = fopen(path, "rb");
	if (!f)
		Py_RETURN_FALSE;
	fread(HR, sizeof(HR), 1, f);
	fclose(f);
	
	Py_RETURN_TRUE;
}

// def compare_against_cards_to_come(self, my_hand_plus_community, test_hand_plus_community, cards_to_come):
static PyObject *crank_compare_against_cards_to_come(PyObject *self, PyObject *args) {
	PyObject *py_my_hand;
	PyObject *py_test_hand;
	PyObject *py_cards_to_come;
	if (!PyArg_ParseTuple(args, "OOO", &py_my_hand, &py_test_hand, &py_cards_to_come))
		return NULL;
	
	int hand_length = PySequence_Length(py_my_hand);
	int my_hand[7];
	convert(py_my_hand, hand_length, my_hand);
	int test_hand[7];
	convert(py_test_hand, hand_length, test_hand);

	int my_count = 0, test_count = 0, invalid_count = 0;
	int len_to_come = PySequence_Length(py_cards_to_come);
	int i = 0;
	for (; i < len_to_come; i++) {
		PyObject *cur_to_come = PySequence_GetItem(py_cards_to_come, i);
		convert(cur_to_come, 7 - hand_length, &test_hand[hand_length]);
		
		// this is a hacky check to see whether there are no duplicate cards
		int k, l;
		for (k = 0; k < hand_length; k++)
			for (l = hand_length; l < 7; l++)
				if (test_hand[k] == test_hand[l])
					goto invalid;
		
		// avoid converting twice by just copying the result
		memcpy(&my_hand[hand_length], &test_hand[hand_length], sizeof(int) * (7 - hand_length));
		
		int my_rank = LookupHandFast(my_hand);
		int test_rank = LookupHandFast(test_hand);
		if (my_rank > test_rank)
			my_count++;
		else if (my_rank < test_rank)
			test_count++;
			
		continue;
		invalid: invalid_count++;
	}
	return Py_BuildValue("(iii)", my_count, test_count, invalid_count);
}

static PyMethodDef crank_funcs[] = {
	{"load_table",	crank_load_table, METH_VARARGS, "load the table"},
	{"get_rank",  crank_get_rank, METH_VARARGS, "get the hand rank for a 7-element list of cards"},
	{"compare_against_cards_to_come",  crank_compare_against_cards_to_come, METH_VARARGS, "compare two known hands against a large quantity of unknown combinations"},
	{NULL, NULL, 0, NULL}	/* Sentinel */
};

PyMODINIT_FUNC initcrank(void) {
	(void)Py_InitModule("crank", crank_funcs);
}
