#pragma once

#ifdef PLATFORM_LINUX
  #define FORMAT_RESET  "\033[0m"
  #define FORMAT_SIZE 11
  #define FORMAT_EMPTY  "\033[0;36m\033[0m"
  #define FORMAT_EMPTY2 "\033[0;36m\033[0m\033[0;36m\033[0m"
  #define FORMAT_GREEN(arg...)  "\033[0;32m"#arg FORMAT_RESET
  #define FORMAT_BLUE(arg...)   "\033[0;34m"#arg FORMAT_RESET
  #define FORMAT_CYAN(arg...)   "\033[0;36m"#arg FORMAT_RESET
  #define FORMAT_RED_BOLD(arg...)    "\033[1;31m"#arg FORMAT_RESET
  #define FORMAT_GREEN_BOLD(arg...)  "\033[1;32m"#arg FORMAT_RESET
  #define FORMAT_YELLOW_BOLD(arg...) "\033[1;33m"#arg FORMAT_RESET
  #define FORMAT_BLUE_BOLD(arg...)   "\033[1;34m"#arg FORMAT_RESET
#else
  #define FORMAT_RESET 
  #define FORMAT_SIZE 0
  #define FORMAT_EMPTY 
  #define FORMAT_EMPTY2
  #define FORMAT_GREEN(arg...)  #arg
  #define FORMAT_BLUE(arg...)   #arg
  #define FORMAT_CYAN(arg...)   #arg
  #define FORMAT_RED_BOLD(arg...)    #arg
  #define FORMAT_GREEN_BOLD(arg...)  #arg
  #define FORMAT_YELLOW_BOLD(arg...) #arg
  #define FORMAT_BLUE_BOLD(arg...)   #arg
#endif
