#pragma once

#include <array>
#include <chrono>
#include <thread>
#include <hidapi/hidapi.h>

//#define discern_arse_elbow

class Ps3Controller
{
    std::atomic<hid_device*> device;
    static constexpr size_t inputBufferSize = 49;

    using dataType = unsigned char;
    using bufferType = std::array<dataType, inputBufferSize>;

    bufferType buffers[2];
    bufferType *input, *output;
    std::mutex dataMutex;

    std::thread workThread;
    std::atomic_bool stopControllerSearch, stopRead, stopWork;

    bool readData();

    void searchForController();
    void readLoop();

    void runWorkThread();
    void stopWorkThread();

    void logMessage(const std::string& m) {
        std::cout << "Ps3Controller " << m << std::endl;
    }

public:
    Ps3Controller();
    ~Ps3Controller();

    static auto& getOne() {
        static std::shared_ptr<Ps3Controller> firstController;

        if(firstController == nullptr) {
            firstController = std::make_shared<Ps3Controller>();
        }

        return firstController;
    }

    //enum class CVal: decltype(inputBuffer)::size_type {
    enum CVal: bufferType::size_type {
        SW_buttons = 0x02,
        NE_buttons = 0x03,
        PS = 0x04,
        L_x = 0x06,
        L_y = 0x07,
        R_x = 0x08,
        R_y = 0x09,
        U = 0x0e,
        R = 0x0f,
        D = 0x10,
        L = 0x11,
        L2 = 0x12,
        R2 = 0x13,
        L1 = 0x14,
        R1 = 0x15,
        Tri = 0x16,
        O = 0x17,
        X = 0x18,
        Sq = 0x19,
        Roll = 0x2a,
        Pitch = 0x2c
    };

    /* Button bits
     * ===========
     * 0x02 & 0x0f : Start | JoyR | JoyL | Select
     * 0x02 & 0xf0 : L | D | R | U
     * 0x03 & 0x0f : R1 | L1 | R2 | L2
     * 0x03 & 0xf0 : Square | X | O | Triangle
    */
    enum BMask: unsigned char {
        Start = 0x08,
        JoyR = 0x04,
        JoyL = 0x02,
        Select = 0x01
    };
    
    auto getCVal(CVal item) -> decltype((*output)[item]) {
        std::lock_guard<std::mutex> dataLock(dataMutex);
        return (*output)[item];
    }

    // no c++14 yet :(
    //const auto& getData();
    const bufferType getData();
};
