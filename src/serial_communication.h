#ifndef SERIAL_COMMUINCATION_H
#define SERIAL_COMMUINCATION_H
#include <boost/asio/serial_port.hpp> 
#include <boost/asio.hpp> 


using namespace boost;
extern uint8_t input_data_buffer[120];
extern uint8_t output_data_buffer[120];


uint8_t serial_connect(const char portname[]);
void serial_disconnect();
void serial_transmit_package(uint8_t length, uint8_t command);
uint8_t serial_receive_package();
void serial_stop_miner();
void serial_reset_miner();
#endif
