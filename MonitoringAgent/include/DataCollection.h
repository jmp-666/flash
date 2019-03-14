#include<fstream>
#include<ctime>
#include<string>
#include<map>
#include<vector>
#include<stdlib.h>

#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H
const int CORE_NUM = 1;                                         // 宿主机CPU核数

struct preContainerData                                         // 前一个时间的容器监控数据
{
    time_t preTime;                                             // 上一次容器状态更新时间戳
    unsigned long long containerTimeSlice, totalTimeSlice;      // 容器使用时间片和机器总的时间片
    unsigned long long diskRead, diskWrite;                     // 磁盘读速率和写速率
    unsigned long long netReceive, netTransmit;                 // 网络接收和发送字节数
};

struct ProcessedData{
    std::string ContainerID;
    time_t Timestamp;
    float CpuLoadAvg;
    float MemLoadAvg;
    float DiskReadAvg;
    float DiskWriteAvg;
    float NetReceiveAvg;
    float NetTransmitAvg;
};


class DataCollection{
private:
    std::fstream fs;
    std::string ContainerID;
    std::map<std::string, preContainerData> ContainerDataList;            // 前一个时间的容器监控数据列表

    void openFile(std::string fileName);                             // 打开指定文件
    void closeFile();                                           // 关闭当前打开的文件

    std::vector<string> split(const char* buffer);                   // 以空格为分隔符来分割字符串
    std::string getConPID();                                         // 获取指定容器id的容器进程号

    unsigned long long readSimpleData(std::string fileName);         // 读取不需要进行分割处理的文件数据流
    unsigned long long readConTimeSlice();                      // 读取容器使用CPU时间片
    unsigned long long readTotalTimeSlice();                    // 读取机器总的CPU时间片
    unsigned long long readMemUsed();                           // 读取容器已经使用的内存
    unsigned long long readMemLimit();                          // 读取机器分配给容器的内存
    unsigned long long *readDiskData();                         // 读取磁盘的读写数据,第一个值为读字节数，第二个值为写字节数
    unsigned long long *readNetData();                          // 读取网络接收和发送字节数，第一个值为接收字节数，第二个值为发送字节数

    float getCpuLoadAvg();                                      // 计算CPU使用率
    float getMemLoadAvg();                                      // 计算内存使用率
    float* getDiskRateAvg();                                    // 计算磁盘读速率和写速率，第一个值为读速率，第二个值为写速率
    float* getNetRateAvg();                                     // 计算网络接收速率和发送速率，第一个值为接受速率，第二个值为发送速率

public:
    DataCollection(std::string ContainerID);                         // 构造函数
    ~DataCollection();                                          // 析构函数，关闭当前打开的文件

    // interface for the ContainerSelection module
    void updateContainerStatus();                               // 添加或者更新指定的容器状态到容器状态列表中
    void eraseContainerStatus();                                // 在容器状态列表中删除指定容器状态

    void processData();                                         // 将指定容器的监控数据转换成json格式
};

#endif