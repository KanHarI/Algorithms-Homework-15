
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

#define DEF_EXCEPTION(superclass, exception) \
    class exception : public superclass { \
    public: \
        exception(std::string message="") : superclass(message) {} \
    };

DEF_EXCEPTION(std::runtime_error, KeyAlreadyExists);
DEF_EXCEPTION(std::runtime_error, KeyNotFound);
DEF_EXCEPTION(std::runtime_error, NotImplemented);

#endif
