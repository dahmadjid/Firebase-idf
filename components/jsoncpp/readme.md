# jsoncpp library

I copied all of the source and header files of the jsoncpp project as they are and put them all in 1 directory and changed the cmakelist so that they work as an ESP-IDF component for easing the pain associated with cmakelist. Otherwise the library is exactly the same.


I tried to use the entire repo as it is with the same cmakelist as done in this xml example: https://github.com/espressif/esp-idf/tree/master/examples/build_system/cmake/import_lib
cmake side worked fine however the linking stage failed for some reason and i was stuck there. issue detailed here: https://www.esp32.com/viewtopic.php?f=13&t=27135
