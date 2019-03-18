#include<Python.h>
#include<sstream>
#include<memory>
#include"json/json.h"
#include"ContainerInfoCollection.h"

using namespace std;

ContainerInfoCollection::ContainerInfoCollection(){}

ContainerInfoCollection::~ContainerInfoCollection(){}

// 在主函数main中单独创建一个线程运行runContainerInfoCollection函数
void ContainerInfoCollection::runContainerInfoCollection(){
    PyObject *pModule, *pFunc;
    PyObject *pValue, *pArgs;

    Py_Initialize();
    if(!Py_IsInitialized()){
        cout << "initialize failed!" << endl;
    }else{
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('../src/python')");

        pModule = PyImport_ImportModule("containerInfo");
        if(!pModule){
            cout << "can't import module!" << endl;
        }else{
            pFunc = PyObject_GetAttrString(pModule, "getContainerInfo");
            if(!pFunc || !PyCallable_Check(pFunc)){
                cout << "can't find function!" << endl;
            }else{
                while(true){
                    time_t sinceTime = time(NULL);
                    time_t untilTime = sinceTime + FOUND_CYCLE;
                    pArgs = PyTuple_New(3);
                    PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", sinceTime));
                    PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", untilTime));
                    PyTuple_SetItem(pArgs, 2, Py_BuildValue("i", FOUND_CYCLE));
                    // 调用函数
                    pValue = PyEval_CallObject(pFunc, pArgs);
                    const char *listStr;
                    PyArg_ParseTuple(pValue, "s", &listStr);
                    stringstream ss;
                    ss.str(listStr);
                    string item;
                    while(getline(ss, item, ';')){
                        string containerID, status;
                        Json::Value jsonRoot;
                        Json::CharReaderBuilder readerBuilder;
                        unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());
                        JSONCPP_STRING errs;
                        bool res = reader->parse(item.c_str(), item.c_str() + item.length(), &jsonRoot, &errs);
                        if (!res || !errs.empty()) 
                        {
                            cout << "parse Json error! " << errs << endl;
                        }else{
                            containerID = jsonRoot["id"].asString();
                            status = jsonRoot["status"].asString();
                            // cout << "id:" << item["id"].asString() << " status:" << item["status"].asString() << endl;
                        }
                        adjustContainerList(containerID, status);
                    }
                }
            }
        }
    }
}