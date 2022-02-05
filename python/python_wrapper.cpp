#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "sockep/SockEPFactory.h"
#include <iostream>

namespace py = pybind11;

int doofus(std::string bindPath, void (*callback)(int, uint8_t*, size_t))
{
    std::cout << "Got something\n";
    return 5;
};

int doofus2(std::string bindPath, std::function<void(int, uint8_t*, size_t)> callback)
{
    std::cout << "Got something\n";
    uint8_t data[6] = {37, 38, 39, 40, 41, 42};
    callback(5, data, 6);
    return 5;
};

char message[10000];

// PYBIND11_MODULE(pysockep, m)
// {
//     // py::class_<sockep::SockEPFactory>(m, "SockEPFactory")
//     //     .def_static("createUnixStreamServerSockEP", &sockep::SockEPFactory::createUnixDgramServerSockEP)
//     //     .def_static("createUnixStreamClientSockEP", &sockep::SockEPFactory::createUnixDgramClientSockEP);

//     py::class_<sockep::IServerSockEP>(m, "IServerSockEP")
//         .def("is_valid", &sockep::IServerSockEP::isValid)
//         .def("start_server", &sockep::IServerSockEP::startServer, py::call_guard<py::gil_scoped_release>())
//         .def("stop_server", &sockep::IServerSockEP::stopServer, py::call_guard<py::gil_scoped_release>())
//         .def("server_running", &sockep::IServerSockEP::serverRunning)
//         .def("send_message_to_client", &sockep::IServerSockEP::sendMessageToClient)
//         .def("get_client_ids", &sockep::IServerSockEP::getClientIds)
//         .def("to_str", &sockep::IServerSockEP::to_str);
        

//     m.def("createUnixStreamServerSockEP", [] (std::string bindPath, py::object &callback)
//         {
//             auto serverCallback = [&callback] (int clientId, uint8_t* msg, size_t msgLen)
//                 {
//                     py::gil_scoped_acquire acquire;
//                     // py::gil_scoped_release release;
//                     memcpy(message, msg, msgLen);
//                     py::bytearray message2(message, msgLen);
//                     std::cout << "Calling python callback\n"; 
//                     callback(clientId, message2);
//                 };
        
//             // return doofus2(bindPath, serverCallback);
//             return sockep::SockEPFactory::createUnixDgramServerSockEP(bindPath, serverCallback);
//         });
//     m.def("createUnixStreamClientSockEP", &sockep::SockEPFactory::createUnixDgramClientSockEP);

//     py::class_<sockep::IClientSockEP>(m, "IClientSockEP")
//         .def("is_valid", &sockep::IClientSockEP::isValid)
//         .def("send_message", [](sockep::IClientSockEP* t, py::bytearray msg)
//             {
//                 uint8_t message[10] = {41, 42, 43, 44, 45, 46};
//                 t->sendMessage(message, 6);
//             }, py::call_guard<py::gil_scoped_release>())
//         .def("get_message", &sockep::IClientSockEP::getMessage, py::call_guard<py::gil_scoped_release>())
//         .def("to_str", &sockep::IClientSockEP::to_str);

// }


    
