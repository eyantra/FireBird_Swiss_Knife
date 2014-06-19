#include <Python.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

HANDLE hSerial;
DWORD dwBytesRead = 0;
void sendString(char name[])
{

if(!WriteFile(hSerial,name, strlen(name), &dwBytesRead, NULL)){
                      printf("unable to write check the port\n");
	}

}


int init(char name[])
{

  
    hSerial = CreateFile(name,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
   if(hSerial==INVALID_HANDLE_VALUE){
             if(GetLastError()==ERROR_FILE_NOT_FOUND){
					printf("could not open port\n");
                                            //serial port does not exist. Inform user.
              }
   }
   
    DCB dcbSerialParams = {0};
    DCB dcbSerial;
dcbSerial.DCBlength=sizeof(dcbSerialParams);
if (!GetCommState(hSerial, &dcbSerialParams)) {
//error getting state
}
 dcbSerialParams.BaudRate=9600;
 dcbSerialParams.ByteSize=8;
 dcbSerialParams.StopBits=ONESTOPBIT;
 dcbSerialParams.Parity=NOPARITY;
 if(!SetCommState(hSerial, &dcbSerialParams)){
                          //error setting serial port state
                          printf("error sending data");
        }
        
COMMTIMEOUTS timeouts={0};
timeouts.ReadIntervalTimeout=50;
timeouts.ReadTotalTimeoutConstant=50;
timeouts.ReadTotalTimeoutMultiplier=10;
timeouts.WriteTotalTimeoutConstant=50;
timeouts.WriteTotalTimeoutMultiplier=10;
if(!SetCommTimeouts(hSerial, &timeouts)){
                             printf("timeout error\n");
}

}


static PyObject *zigbeeError;
static PyObject * zigbee_sendString(PyObject *self, PyObject *args)
{
    const char *command;
    int sts,i;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;

    sendString(command);
    if (sts < 0) {
        PyErr_SetString(zigbeeError, "sendString command failed");
        return NULL;
    }
    return PyLong_FromLong(sts);
}

static PyObject * zigbee_init(PyObject *self, PyObject *args)
{
    const char *command;
    int sts,i;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;

    init(command);
    if (sts < 0) {
        PyErr_SetString(zigbeeError, "sendString command failed");
        return NULL;
    }
    return PyLong_FromLong(sts);
}

PyDoc_STRVAR(zigbee_doc,"This is a template module just for instruction.");
static PyMethodDef zigbeeMethods[] = {
    {"sendString",  zigbee_sendString, METH_VARARGS,	
     "send a string through port"},
	 {"init",  zigbee_init, METH_VARARGS,	
     "open a communication port"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};



static struct PyModuleDef zigbeemodule = {
   PyModuleDef_HEAD_INIT,
   "zigbee",   /* name of module */
   zigbee_doc, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   zigbeeMethods
};




PyMODINIT_FUNC PyInit_zigbee(void)
{
    PyObject *m;

    m = PyModule_Create(&zigbeemodule);
    if (m == NULL)
        return NULL;

    zigbeeError = PyErr_NewException("zigbee.error", NULL, NULL);
    Py_INCREF(zigbeeError);
    PyModule_AddObject(m, "error", zigbeeError);
    return m;
}







