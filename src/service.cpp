#include <easy_tcp/service.h>

namespace easy_tcp {
    Service::~Service() {
        if (m_threadHandler != nullptr) {
            m_threadHandler->detach();
            delete m_threadHandler;
            m_threadHandler = nullptr;
        }
    }

    bool Service::operator==(const Service &other) {
        if ((this->fd == other.fd) &&
            (this->fd == other.fd)) {
            return true;
        }
        return false;
    }
}