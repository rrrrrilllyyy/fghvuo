#include "serial_communication.h"
#include "util.h"

#define BEGIN(a)            ((char*)&(a))


using namespace boost;


uint8_t input_data_buffer[120];
uint8_t output_data_buffer[120];

asio::io_service miner_io;
asio::serial_port port(miner_io);

uint8_t serial_connect(const char portname[])
{
	printf("\n\nISOPEN?\n\n");
	if( !port.is_open() )
	{
		printf("\n\nNO!\n\n");
    port.open(portname);
    //port.set_option(asio::serial_port_base::baud_rate(1000000));
	}
	if( !port.is_open() )
	{
		return 0;
	}
	return 1;
}
void serial_disconnect()
{
	printf("\n\nPORT CLOSED\n\n");
	port.close();
}
void serial_transmit_package(uint8_t length, uint8_t command)
{
	char portnamearr[100];
	std::string portnamestr=GetArg("-minerport","");
	strcpy(portnamearr, portnamestr.c_str());
	
	
	serial_connect(portnamearr);
	uint8_t ack;
	for(uint8_t i = length - 2; i > 1; i --)
	{
		output_data_buffer[i] = output_data_buffer[i - 2];
	}
	output_data_buffer[0] = length;
	output_data_buffer[1] = command;
	uint64_t checksum_midstate = 0;
	for(uint8_t i = 1; i < length - 1; i ++)
	{
		checksum_midstate = checksum_midstate + i * output_data_buffer[i];
	}
    output_data_buffer[length - 1] = checksum_midstate % 128 + 42;
	
	asio::write(port, asio::buffer(output_data_buffer,length));

	
	
	printf("\n\nDATA SENT\n\n");

	
	asio::read(port, asio::buffer(&ack,1));
	if(ack == 0x61)
	{
		printf("\n\nACK RECEIVED\n\n");
	}
	else
	{
		printf("\n\nERROR: [ %lu ]dump:", ack);
		while(1)
			{
				asio::read(port, asio::buffer(&ack,1));
				printf("[ %lu ]", ack);
			}
	}
	
}

uint8_t serial_receive_package()
{
	printf("\n\nReceive\n\n");
	uint8_t length;
	asio::read(port, asio::buffer(&length,1));
	printf("\n\nGot Length!\n\n");
	printf("\n\n");
	asio::read(port, asio::buffer(&input_data_buffer,length - 1));
	printf("\n\ngot data!\n\n");
    for(uint8_t i = length - 1; i > 0; i --)
	{
		input_data_buffer[i] = input_data_buffer[i - 1];
    }
    input_data_buffer[0] = length - 2;

    uint64_t checksum_midstate = 0;
    for(uint8_t i = 1; i < length - 1; i ++)
    {
        checksum_midstate = checksum_midstate + i * input_data_buffer[i];
    }
    if(input_data_buffer[length - 1] == checksum_midstate % 128 + 42)
    {
        return 1;
    }
    return 0;
}


void serial_reset_miner()
{
	uint8_t flush = 0;
	for(uint8_t i = 0; i < 150; i ++)
	{
		asio::write(port, asio::buffer(BEGIN(flush),1)); // write 150 zero bytes to flush miner rx buffer, will be ignored by miner once buffer empty!
	}
	
}	
