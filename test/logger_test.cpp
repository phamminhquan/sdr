#include <iostream>
#include <stdlib.h>
#include "../src/logger.h"
#include <inttypes.h>
using namespace sdr;


int main() {

  std::ostream &objOstream = std::cout;
  //LogMessage debug_msg(LOG_DEBUG);
  LogMessage info_msg(LOG_INFO);
  //LogMessage warning_msg(LOG_WARNING);
  //LogMessage error_msg(LOG_ERROR);
  //debug_msg << "Debug message" << std::endl;
  info_msg << "Info message" << std::endl;
  //warning_msg << "Warning message" << std::endl;
  //error_msg << "Error message" << std::endl;
  StreamLogHandler log_handler(objOstream, LOG_INFO);
  Logger::get().addHandler(&log_handler);
  Logger::get().log(info_msg);

  return 0;
}
