#include <iostream>

#include "logger.hpp"

int
main()
{
    Logger log;

    log.log(Logger::WARN) << "These messages are only color-coded on Linux!!!" << Logger::MessageStream::endl;

    log.log(Logger::DEBUG) << "These won't print by default!" << Logger::MessageStream::endl;
    log.log(Logger::INFO) << "These won't print by default!" << Logger::MessageStream::endl;

    log.set_print_level(Logger::DEBUG);

    log.log(Logger::DEBUG) << "Now they will!" << Logger::MessageStream::endl;
    log.log(Logger::INFO) << "Now they will!" << Logger::MessageStream::endl;

    log.log(Logger::INFO) << "The endl is not strictly nessicary. These messages get queued when the MessageStream unique_ptr frees itself, and the endl forces them out of scope.";

    log.log(Logger::DEBUG) << "Testing DEBUG!" << Logger::MessageStream::endl;
    log.log(Logger::INFO) << "Testing INFO!" << Logger::MessageStream::endl;
    log.log(Logger::WARN) << "Testing WARN!" << Logger::MessageStream::endl;
    log.log(Logger::ERROR) << "Testing ERROR!" << Logger::MessageStream::endl;
    log.log(Logger::FATAL) << "Testing FATAL!" << Logger::MessageStream::endl;

    for (int i=0; i<5; i++)
        log.log(Logger::ERROR) << "Testing numbers! (" << i << ")" << Logger::MessageStream::endl;

    return 0;
}
