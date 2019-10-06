#ifndef __SDR_LOGGER_HH__
#define __SDR_LOGGER_HH__

#include <string>
#include <sstream>
#include <list>

namespace sdr {

  // DIFFERENT TYPES of LOG MESSAGES
  typedef enum {
    LOG_DEBUG = 0, // Everything that may be of interest
    LOG_INFO, // Messages about state changes
    LOG_WARNING, // Non-critical errors
    LOG_ERROR // Critical error
  } LogLevel;

  // Log Message
  class LogMessage: public std::stringstream {
    public:
      // Constructor with log level and message
      LogMessage(LogLevel level, const std::string &msg="");

      // Contructor from another log message
      LogMessage(const LogMessage &other);

      // Destructor
      virtual ~LogMessage();

      // Methods
      // Returns level of message
      LogLevel level() const;

      // Returns the message
      inline std::string message() const { return this->str(); }

    protected:
      LogLevel _level;
  };

  // Log Handler (base class)
  class LogHandler {
    protected:
      // Hidden constructor
      LogHandler();
    
    public:
      // Destructor
      virtual ~LogHandler();
      // Needs to be implemented by sub-classes to handle log messages
      virtual void handle(const LogMessage &msg) = 0;
  };

  // Serializes log message into the specified stream
  class StreamLogHandler: public LogHandler {
    public:
      // Constructor with specified stream, messages are serializes into
      // level specifies the minimum log level of the message being serialized
      StreamLogHandler(std::ostream &stream, LogLevel level);

      // Destructor
      virtual ~StreamLogHandler();

      // Handle methods
      virtual void handle(const LogMessage &msg);

    protected:
      // Output stream
      std::ostream &_stream;

      // Minimum log level
      LogLevel _level;
  };

  // Logger class
  class Logger {
    protected:
      // Hidden constructor
      Logger();
    
    public:
      // Destructor
      virtual ~Logger();

      // Returns the singleton instance of the logger
      static Logger &get();

      // Log method: log a message
      void log(const LogMessage &message);

      // message handler
      void addHandler(LogHandler *handler);

    protected:
      // the singleton instance
      static Logger *_instance;

      // registered handler
      std::list<LogHandler*> _handler;
  };

}
#endif
