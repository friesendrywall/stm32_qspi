
#include "qspi/Serialize.h"

//#define ENABLE_PRINT_DEBUG

#define EXT_MOD

/*******************************************************************************
Function:     Serialize(const CharStream* char_stream_send,
					CharStream* char_stream_recv,
					SpiMasterConfigOptions optBefore,
					SpiMasterConfigOptions optAfter
				)
Arguments:    char_stream_send, the char stream to be sent from the SPI master to
              the Flash memory, usually contains instruction, address, and data to be
              programmed.
              char_stream_recv, the char stream to be received from the Flash memory
              to the SPI master, usually contains data to be read from the memory.
              optBefore, configurations of the SPI master before any transfer/receive
              optAfter, configurations of the SPI after any transfer/receive
Return Values:TRUE
Description:  This function can be used to encapsulate a complete transfer/receive
              operation
Pseudo Code:
   Step 1  : perform pre-transfer configuration
   Step 2  : perform transfer/ receive
   Step 3  : perform post-transfer configuration
*******************************************************************************/
SPI_STATUS Serialize_SPI(const CharStream* char_stream_send,
                         CharStream* char_stream_recv,
                         SpiConfigOptions optBefore,
                         SpiConfigOptions optAfter
                        )
{

	uint8_t *char_send, *char_recv;
	uint16_t rx_len = 0, tx_len = 0;

#ifdef ENABLE_PRINT_DEBUG
	int i;
	printf("\nSEND: ");
	for(i = 0; i < char_stream_send->length; i++)
		printf(" 0x%x ", char_stream_send->pChar[i]);
	printf("\n");
#endif

	tx_len = char_stream_send->length;
	char_send = char_stream_send->pChar;

	if (NULL_PTR != char_stream_recv) {
		rx_len = char_stream_recv->length;
		char_recv = char_stream_recv->pChar;
	}


#if 0
	ConfigureSpi(optBefore);


	while (tx_len-- > 0) {
		WR_R(SPIWRFIFO,  *(char_send++));
		CHECK_BSY;
		RD_R(SPIRDFIFO);
	}

	while (rx_len-- > 0) {
		WR_R(SPIWRFIFO,  DUMMY_BYTE);
		CHECK_BSY;

		if (CHECK_RX_FIFO)
			*char_recv++ = RD_R(SPIRDFIFO);
		else
			rx_len++;
	}


#ifdef ENABLE_PRINT_DEBUG
	printf("\nRECV: ");
	for(i = 0; i < char_stream_recv->length; i++)
		printf(" 0x%x ", char_stream_recv->pChar[i]);
	printf("\n");
#endif

	ConfigureSpi(optAfter);
#endif

	return RetSpiSuccess;
}

void four_byte_addr_ctl(int enable) {
#if 0
	if(enable)
		FOUR_BYTE_ENABLE;

	if(!enable)
		FOUR_BYTE_DISABLE;
#endif
}