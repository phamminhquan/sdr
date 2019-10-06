#include "logger.h"

using namespace sdr;

// Log Message
// Constructors
LogMessage::LogMessage(LogLevel level, const std::string &msg)
  : std::stringstream(), _level(level)
{
  (*this) << msg;
}

LogMessage::LogMessage(const LogMessage &other)
  : std::stringstream(), _level(other._level)
{
  (*this) << other.message();
}

// Destructor
LogMessage::~LogMessage() {}

// Methods
LogLevel
LogMessage::level() const {
  return _level;
}

// Log Handler Constructor
LogHandler::LogHandler() {}

// Log Handler Destructor
LogHandler::~LogHandler() {}

// Stream Log Handler
// Constructor
StreamLogHandler::StreamLogHandler(std::ostream &stream, LogLevel level)
  : LogHandler(), _stream(stream), _level(level)
{}

// Destructor
StreamLogHandler::~StreamLogHandler()
{}

// Method handle
void StreamLogHandler::handle(const LogMessage &msg) {
  if (msg.level() < _level) { return; }
  switch (msg.level()) {
    case LOG_DEBUG:
      _stream << "DEBUG: ";
      break;
    case LOG_INFO:
      _stream << "INFO: ";
      break;
    case LOG_WARNING:
      _stream << "WARNING: ";
      break;
    case LOG_ERROR:
      _stream << "ERROR: ";
      break;
  }
  _stream << msg.message() << std::endl;
}

// Logger
Logger *Logger::_instance = 0;

// Constructor
Logger::Logger()
  : _handler()
{}

// Destructor
Logger::~Logger() {
  std::list<LogHandler*>::iterator item = _handler.begin();
  for (; item != _handler.end(); item++) {
    delete (*item);
  }
  _handler.clear();
}

Logger & Logger::get() {
  if (_instance == 0) { _instance = new Logger(); }
  return *_instance;
}

void Logger::addHandler(LogHandler *handler) {
  _handler.push_back(handler);
}

void Logger::log(const LogMessage &message) {
  std::list<LogHandler*>::iterator item = _handler.begin();
  for (; item != _handler.end(); item++) {
    (*item)->handle(message);
  }
}
