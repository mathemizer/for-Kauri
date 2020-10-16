#!/bin/sh
#%SrcAddr% - адрес папки с исходниками
#%DstAddr% - адрес папки куда будет скомпилирован исполняемый фаил
g++ -c %SrcAddr%\Kauri_task_qmake\asn1_resolver.cpp
g++ -c %SrcAddr%\Kauri_task_qmake\my_semaphore.cpp
g++ -c %SrcAddr%\Kauri_task_qmake\server_unit_cl.cpp
g++ -c %SrcAddr%\Reimeno\Documents\Kauri_task_qmake\server_unit_method_win.cpp
g++ -c %SrcAddr%\Kauri_task_qmake\main.cpp
g++ -o %DstAddr%\Kauri_task_server main.o server_unit_method_win.o server_unit_cl.o my_semaphore.o asn1_resolver.o -static -libgcc -libstdc++ -lwinpthread -lws2_32
 