#include "logger.hpp"

#include <iostream>
#include <iomanip>

Logger::MessageStream::~MessageStream()
{
    std::shared_ptr<Message> message = std::shared_ptr<Message> (new Message);
    message->log_level = log_level;
    message->thread_id = std::this_thread::get_id();
    message->msg = stream.str();
    flush_queue->push(message);
}

Logger::Logger()
    :print_all(false),
     thread_stop(false),
     msg_queue(new Queue<std::shared_ptr<Message>>())
{
    object_thread = std::shared_ptr<std::thread> (new std::thread(&Logger::run, this));

    std::shared_ptr<Message> open_message(new Message());
    open_message->log_level = DEBUG;
    open_message->msg = "Logger starting up...";
    msg_queue->push(open_message);
}

Logger::~Logger()
{
    thread_stop = true;
    std::shared_ptr<Message> close_message(new Message());
    close_message->log_level = DEBUG;
    close_message->msg = "Logger shutting down...";
    msg_queue->push(close_message);
    object_thread->join();
}

void Logger::set_print_level(level level)
{
    std::shared_ptr<Message> message = std::shared_ptr<Message> (new Message);
    message->thread_id = std::this_thread::get_id();
    message->set_print_level = level;
    msg_queue->push(message);
}

std::unique_ptr<Logger::MessageStream> Logger::log(Logger::level log_level)
{
    std::unique_ptr<MessageStream> msg(new MessageStream);
    msg->log_level = log_level;
    msg->flush_queue = msg_queue;
    return msg;
}

void Logger::run()
{
    std::shared_ptr<Message> msg;
    while(!thread_stop || !msg_queue->empty())
    {
        msg_queue->pop(msg);
        if(msg->set_print_level != NONE)
        {
            log_levels[msg->thread_id] = msg->set_print_level;
            continue;
        }
        if(msg->log_level <= get_level(msg->thread_id) || print_all == true)
            print_msg(msg);
    }
}

void Logger::print_msg(std::shared_ptr<Message> message)
{
    std::cout << get_prefix(message) << message->msg << std::endl;
}

int Logger::thread_id(std::thread::id thread_id)
{
    int next = 0;
    static std::map<std::thread::id, int> ids;
    if(ids.find(thread_id) == ids.end())
        ids[thread_id] = next++;
    return ids[thread_id];
}

std::string Logger::get_prefix(std::shared_ptr<Message> message)
{
    std::ostringstream prefix;

    #ifdef __linux__
    switch(message->log_level)
    {
    case DEBUG:
        prefix << "[\033[34mDEBUG\033[0m]";
        break;
    case INFO:
        prefix << "[\033[32mINFO \033[0m]";
        break;
    case WARN:
        prefix << "[\033[33mWARN \033[0m]";
        break;
    case ERROR:
        prefix << "[\033[31mERROR\033[0m]";
        break;
    case FATAL:
        prefix << "[\033[35mFATAL\033[0m]";
        break;
    default:
        prefix << "[\033[36mUNKNOWN\033[0m]";
        break;
    }

    prefix << "[Thread: \033[32m" << thread_id(message->thread_id) << "\033[0m] ";
    #else
    switch(message->log_level)
    {
    case DEBUG:
        prefix << "[DEBUG]";
        break;
    case INFO:
        prefix << "[INFO ]";
        break;
    case WARN:
        prefix << "[WARN ]";
        break;
    case ERROR:
        prefix << "[ERROR]";
        break;
    case FATAL:
        prefix << "[FATAL]";
        break;
    default:
        prefix << "[UNKNOWN]";
        break;
    }

    prefix << "[Thread: " << thread_id(message->thread_id) << "] ";
    #endif

    return prefix.str();
}

Logger::level Logger::get_level(std::thread::id thread_id)
{
    std::map<std::thread::id, level>::iterator it;
    it = log_levels.find(thread_id);
    if(it == log_levels.end())
    {
        log_levels.insert(std::pair<std::thread::id, level>(thread_id, WARN));
        return WARN;
    } else {
        return it->second;
    }
}

void operator<<(std::unique_ptr<Logger::MessageStream> msg_stream, Logger::MessageStream::MessageStreamTerminator msg_term)
{
    msg_term(msg_stream);
    msg_stream.reset();
}

std::unique_ptr<Logger::MessageStream> operator<<(std::unique_ptr<Logger::MessageStream> msg_stream, Logger::MessageStream::MessageStreamManipulator msg_man)
{
    msg_man(msg_stream);
    return msg_stream;
}

void Logger::MessageStream::endl(std::unique_ptr<MessageStream>&)
{

}
