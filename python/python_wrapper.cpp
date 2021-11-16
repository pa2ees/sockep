#include <pybind11/pybind11.h>

#include "sockep/SockEPFactory.h"

namespace py = pybind11;

PYBIND11_MODULE(pysockep, m)
{
    // py::class_<sockep::SockEPFactory>(m, "SockEPFactory")
    //     .def_static("createUnixStreamServerSockEP", &sockep::SockEPFactory::createUnixDgramServerSockEP)
    //     .def_static("createUnixStreamClientSockEP", &sockep::SockEPFactory::createUnixDgramClientSockEP);
    
    m.def("createUnixStreamServerSockEP", &sockep::SockEPFactory::createUnixDgramServerSockEP);
    m.def("createUnixStreamClientSockEP", &sockep::SockEPFactory::createUnixDgramClientSockEP);

    py::class_<sockep::IClientSockEP>(m, "IClientSockEP")
        .def("is_valid", &sockep::IClientSockEP::isValid)
        .def("send_message", &sockep::IClientSockEP::sendMessage)
        .def("get_message", &sockep::IClientSockEP::getMessage)
        .def("to_str", &sockep::IClientSockEP::to_str);

}


    
