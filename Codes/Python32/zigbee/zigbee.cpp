#include <Python.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

HANDLE hSerial;
DWORD dwBytesRead = 0;
DWORD dwBytesWrote = 0;




void getStringTemp(char *name)
{

if(ReadFile(hSerial,name, 1,&dwBytesRead, NULL)==0){
                      printf("unable to read check the port\n");
	}
    if((name[0]>='A' && name[0]<='z') || (name[0]>='0' && name[0] <='9') || name[0]=='#' || name[0]==' ')
    {                       
    }
    else name[0]='\0';
	name[1]='\0';
}

void getString(char *name, int n)
{
     int i;
     char temp[2];
	 while(1)
	 {	getStringTemp(temp);
		if(temp[0]=='#') break;
	//	printf("waiting for # got %c",temp[0]);
	 }
     for(i=0;i<n-1;i++)
     {
	// printf("reading..");
      getStringTemp(temp);
      if(strlen(temp)==0) {i--; continue;}
      if(temp[0]=='#') {name[i]='\0'; return;}
      name[i]=temp[0];
     }
     name[n]='\0';
     return;
}


void sendString(char name[])
{

if(!WriteFile(hSerial,name, strlen(name), &dwBytesWrote, NULL)){
                      printf("unable to write check the port\n");
	}

}


void init(char name[])
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

static PyObject * zigbee_getString(PyObject *self, PyObject *args)
{
    char *command,str[51];
    getString(str,50);
    return PyBytes_FromString(str);
}

static PyObject * zigbee_sendString(PyObject *self, PyObject *args)
{
    char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sendString(command);
    return PyLong_FromLong(sts);
}

static PyObject * zigbee_init(PyObject *self, PyObject *args)
{
    char *command;
    int sts;

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
     {"getString",  zigbee_getString, METH_VARARGS,	
     "get a string from port"},
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







