#ifndef BLOCKS_LOGGER_H
#define BLOCKS_LOGGER_H

#include <thread>
#include <string>
#include "queue.hpp"

#include <sstream>
#include <string>
#include <map>

class Logger {
public:
    enum level {DEBUG=4, INFO=3, WARN=2, ERROR=1, FATAL=0, NONE=99};

    struct Message {
    public:
        std::string msg;
        std::thread::id thread_id;
        level log_level;
        level set_print_level = NONE;
    };

    class MessageStream {
    public:
        ~MessageStream();

        typedef MessageStream& (*MessageStreamManipulator)(std::unique_ptr<MessageStream>&);
        typedef void (*MessageStreamTerminator)(std::unique_ptr<MessageStream>&);
        static void endl(std::unique_ptr<MessageStream>&);

        template <typename TT>
        friend std::unique_ptr<MessageStream> operator<<(std::unique_ptr<MessageStream> msg_stream, TT obj)
        {
            msg_stream->stream << obj;
            return msg_stream;
        };

        friend std::unique_ptr<MessageStream> operator<<(std::unique_ptr<MessageStream>, MessageStreamManipulator);
        friend void operator<<(std::unique_ptr<MessageStream>, MessageStreamTerminator);

        level log_level;
        std::ostringstream stream;
        std::shared_ptr<Queue<std::shared_ptr<Message>>> flush_queue;
    };

    explicit Logger();
    ~Logger();

	static Logger stdout;

    void set_print_level(level);

    std::unique_ptr<Logger::MessageStream> log(level log_level);

private:
    Logger& operator=(const Logger&);

    void run();
    void print_msg(std::shared_ptr<Message> MessageStream);
    int thread_id(std::thread::id);
    std::string get_prefix(std::shared_ptr<Message> message);
    level get_level(std::thread::id);

    bool print_all;

    std::map<std::thread::id, level> log_levels;
    bool thread_stop;
    std::shared_ptr<std::thread> object_thread;
    std::shared_ptr<Queue<std::shared_ptr<Message>>> msg_queue;
};

#endif
