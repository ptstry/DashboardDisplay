/*
 * Can.c
 *
 * Created: 05/07/2018 23:25:12
 *  Author: Piotr
 */ 

#include <asf.h>
#include <string.h>
#include <conf_mcan.h>

#include "Memory.h"

static void configure_mcan(void);

/* module_inst */
static struct mcan_module mcan_instance;

/* mcan_filter_setting */
#define MCAN_RX_STANDARD_FILTER_INDEX_0    0
#define MCAN_RX_STANDARD_FILTER_INDEX_1    1
#define MCAN_RX_STANDARD_FILTER_ID_0     0x45A
#define MCAN_RX_STANDARD_FILTER_ID_0_BUFFER_INDEX     2
#define MCAN_RX_STANDARD_FILTER_ID_1     1000
#define MCAN_RX_EXTENDED_FILTER_INDEX_0    0
#define MCAN_RX_EXTENDED_FILTER_INDEX_1    1
#define MCAN_RX_EXTENDED_FILTER_ID_0     0x100000A5
#define MCAN_RX_EXTENDED_FILTER_ID_0_BUFFER_INDEX     1
#define MCAN_RX_EXTENDED_FILTER_ID_1     0x10000096

/* mcan_transfer_message_setting */
#define MCAN_TX_BUFFER_INDEX    0
static uint8_t tx_message_0[CONF_MCAN_ELEMENT_DATA_SIZE];
static uint8_t tx_message_1[CONF_MCAN_ELEMENT_DATA_SIZE];

/* mcan_receive_message_setting */
static volatile uint32_t standard_receive_index = 0;
static volatile uint32_t extended_receive_index = 0;
//COMPILER_SECTION(".ram_nocache")
//COMPILER_ALIGNED(32) 
//COMPILER_SECTION(".ram_usbnocache")
//__attribute__((__section__(".ram_usbnocache")))
#define COMPILER_SECTION(a)    __attribute__((__section__(a)))

// #ifdef MPU_HAS_NOCACHE_REGION
// COMPILER_SECTION(".ram_usbnocache")
// #else
// COMPILER_ALIGNED(4)
// #endif // MPU_HAS_NOCACHE_REGION
static struct mcan_rx_element_fifo_0 rx_element_fifo_0;

static struct mcan_rx_element_fifo_1 rx_element_fifo_1;
static struct mcan_rx_element_buffer rx_element_buffer;


static void configure_mcan(void) {
    uint32_t i;
    /* Initialize the memory. */
    for (i = 0; i < CONF_MCAN_ELEMENT_DATA_SIZE; i++) {
        tx_message_0[i] = i;
        tx_message_1[i] = i + 0x80;
    }

    /* Initialize the module. */
    struct mcan_config config_mcan;
    mcan_get_config_defaults(&config_mcan);
    
    mcan_init(&mcan_instance, MCAN_MODULE, &config_mcan);
    
    //mcan_set_baudrate(MCAN_MODULE, 1000000);
    
    mcan_start(&mcan_instance);
    
    /* Enable interrupts for this MCAN module */
    //irq_register_handler(MCAN0_IRQn, 5);
    //mcan_enable_interrupt(&mcan_instance, MCAN_FORMAT_ERROR | MCAN_ACKNOWLEDGE_ERROR | MCAN_BUS_OFF);


}

/**
 * \brief set receive standard MCAN ID, dedicated buffer
 *
 */
static void mcan_set_standard_filter_0(void)
{
	struct mcan_standard_message_filter_element sd_filter;

	mcan_get_standard_message_filter_element_default(&sd_filter);
	sd_filter.S0.bit.SFID2 = MCAN_RX_STANDARD_FILTER_ID_0_BUFFER_INDEX;
	sd_filter.S0.bit.SFID1 = MCAN_RX_STANDARD_FILTER_ID_0;
	sd_filter.S0.bit.SFEC =
			MCAN_STANDARD_MESSAGE_FILTER_ELEMENT_S0_SFEC_STRXBUF_Val;

	mcan_set_rx_standard_filter(&mcan_instance, &sd_filter,
			MCAN_RX_STANDARD_FILTER_INDEX_0);
	mcan_enable_interrupt(&mcan_instance, MCAN_RX_BUFFER_NEW_MESSAGE);
}

/**
 * \brief set receive standard MCAN ID,FIFO buffer.
 *
 */
static void mcan_set_standard_filter_1(void)
{
	struct mcan_standard_message_filter_element sd_filter;

	mcan_get_standard_message_filter_element_default(&sd_filter);
	sd_filter.S0.bit.SFID1 = MCAN_RX_STANDARD_FILTER_ID_1;

	mcan_set_rx_standard_filter(&mcan_instance, &sd_filter,
			MCAN_RX_STANDARD_FILTER_INDEX_1);
	mcan_enable_interrupt(&mcan_instance, MCAN_RX_FIFO_0_NEW_MESSAGE);
}

/**
 * \brief set receive extended MCAN ID, dedicated buffer
 *
 */
static void mcan_set_extended_filter_0(void)
{
	struct mcan_extended_message_filter_element et_filter;

	mcan_get_extended_message_filter_element_default(&et_filter);
	et_filter.F0.bit.EFID1 = MCAN_RX_EXTENDED_FILTER_ID_0;
	et_filter.F0.bit.EFEC =
			MCAN_EXTENDED_MESSAGE_FILTER_ELEMENT_F0_EFEC_STRXBUF_Val;
	et_filter.F1.bit.EFID2 = MCAN_RX_EXTENDED_FILTER_ID_0_BUFFER_INDEX;

	mcan_set_rx_extended_filter(&mcan_instance, &et_filter,
			MCAN_RX_EXTENDED_FILTER_INDEX_0);
	mcan_enable_interrupt(&mcan_instance, MCAN_RX_BUFFER_NEW_MESSAGE);
}

/**
 * \brief set receive extended MCAN ID,FIFO buffer.
 *
 */
static void mcan_set_extended_filter_1(void)
{
	struct mcan_extended_message_filter_element et_filter;

	mcan_get_extended_message_filter_element_default(&et_filter);
	et_filter.F0.bit.EFID1 = MCAN_RX_EXTENDED_FILTER_ID_1;

	mcan_set_rx_extended_filter(&mcan_instance, &et_filter,
			MCAN_RX_EXTENDED_FILTER_INDEX_1);
	mcan_enable_interrupt(&mcan_instance, MCAN_RX_FIFO_1_NEW_MESSAGE);
}

/**
 * \brief send standard MCAN message,
 *
 *\param id_value standard MCAN ID
 *\param *data  content to be sent
 *\param data_length data length code
 */
static void mcan_send_standard_message(uint32_t id_value, uint8_t *data,
		uint32_t data_length)
{
	uint32_t i;
	struct mcan_tx_element tx_element;

	mcan_get_tx_buffer_element_defaults(&tx_element);
	tx_element.T0.reg |= MCAN_TX_ELEMENT_T0_STANDARD_ID(id_value);
	tx_element.T1.bit.DLC = data_length;
	for (i = 0; i < data_length; i++) {
		tx_element.data[i] = *data;
		data++;
	}

	mcan_set_tx_buffer_element(&mcan_instance, &tx_element,
			MCAN_TX_BUFFER_INDEX);
	mcan_tx_transfer_request(&mcan_instance, 1 << MCAN_TX_BUFFER_INDEX);
}

/**
 * \brief send extended MCAN message,
 *
 *\param id_value extended MCAN ID
 *\param *data  content to be sent
 *\param data_length data length code
 */
static void mcan_send_extended_message(uint32_t id_value, uint8_t *data,
		uint32_t data_length)
{
	uint32_t i;
	struct mcan_tx_element tx_element;

	mcan_get_tx_buffer_element_defaults(&tx_element);
	tx_element.T0.reg |= MCAN_TX_ELEMENT_T0_EXTENDED_ID(id_value) |
			MCAN_TX_ELEMENT_T0_XTD;
	tx_element.T1.bit.DLC = data_length;
	for (i = 0; i < data_length; i++) {
		tx_element.data[i] = *data;
		data++;
	}

	mcan_set_tx_buffer_element(&mcan_instance, &tx_element,
			MCAN_TX_BUFFER_INDEX);
	mcan_tx_transfer_request(&mcan_instance, 1 << MCAN_TX_BUFFER_INDEX);
}

/**
 * \brief Interrupt handler for MCAN,
 *   inlcuding RX,TX,ERROR and so on processes.
 */
void MCAN0_Handler(void)
{
	volatile uint32_t status, i, rx_buffer_index;
	status = mcan_read_interrupt_status(&mcan_instance);

	if (status & MCAN_RX_BUFFER_NEW_MESSAGE) {  
		mcan_clear_interrupt_status(&mcan_instance, MCAN_RX_BUFFER_NEW_MESSAGE);
		for (i = 0; i < CONF_MCAN0_RX_BUFFER_NUM; i++) {
			if (mcan_rx_get_buffer_status(&mcan_instance, i)) {
				rx_buffer_index = i;
				mcan_rx_clear_buffer_status(&mcan_instance, i);
				mcan_get_rx_buffer_element(&mcan_instance, &rx_element_buffer,
				rx_buffer_index);
				if (rx_element_buffer.R0.bit.XTD) {
					printf("\n\r Extended message received in Rx buffer. The received data is: \r\n");
				} else {
					printf("\n\r Standard message received in Rx buffer. The received data is: \r\n");
				}
				for (i = 0; i < rx_element_buffer.R1.bit.DLC; i++) {
                    rx_element_buffer.data[i];
					printf("  %d",rx_element_buffer.data[i]);
                    rx_element_buffer.data[i]=0;
				}
				printf("\r\n\r\n");
			}
		}
	}

	if (status & MCAN_RX_FIFO_0_NEW_MESSAGE) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_RX_FIFO_0_NEW_MESSAGE);
		mcan_get_rx_fifo_0_element(&mcan_instance, &rx_element_fifo_0,
				standard_receive_index);
		mcan_rx_fifo_acknowledge(&mcan_instance, 0,
				standard_receive_index);
		standard_receive_index++;
		if (standard_receive_index == CONF_MCAN0_RX_FIFO_0_NUM) {
			standard_receive_index = 0;
		}

		printf("\n\r Standard message received in FIFO 0. The received data is: \r\n");
		for (i = 0; i < rx_element_fifo_0.R1.bit.DLC; i++) {
			printf("  %d",rx_element_fifo_0.data[i]);
		}
		printf("\r\n\r\n");
	}

	if (status & MCAN_RX_FIFO_1_NEW_MESSAGE) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_RX_FIFO_1_NEW_MESSAGE);
		mcan_get_rx_fifo_1_element(&mcan_instance, &rx_element_fifo_1,
				extended_receive_index);
		mcan_rx_fifo_acknowledge(&mcan_instance, 0,
				extended_receive_index);
		extended_receive_index++;
		if (extended_receive_index == CONF_MCAN0_RX_FIFO_1_NUM) {
			extended_receive_index = 0;
		}

		printf("\n\r Extended message received in FIFO 1. The received data is: \r\n");
		for (i = 0; i < rx_element_fifo_1.R1.bit.DLC; i++) {
			printf("  %d",rx_element_fifo_1.data[i]);
		}
		printf("\r\n\r\n");
	}

	if (status & MCAN_BUS_OFF) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_BUS_OFF);
		mcan_stop(&mcan_instance);
		printf(": MCAN bus off error, re-initialization. \r\n\r\n");
		configure_mcan();
	}

	if (status & MCAN_ACKNOWLEDGE_ERROR) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_ACKNOWLEDGE_ERROR);
		printf("Protocol ACK error, please double check the clock in two boards. \r\n\r\n");
	}

	if (status & MCAN_FORMAT_ERROR) {
		mcan_clear_interrupt_status(&mcan_instance, MCAN_FORMAT_ERROR);
		printf("Protocol format error, please double check the clock in two boards. \r\n\r\n");
	}
}    


void can_receive() {
    volatile uint32_t status, i, rx_buffer_index;
    status = mcan_read_interrupt_status(&mcan_instance);

    if (status & MCAN_RX_BUFFER_NEW_MESSAGE) {
        
        mcan_clear_interrupt_status(&mcan_instance, MCAN_RX_BUFFER_NEW_MESSAGE);
        for (i = 0; i < CONF_MCAN0_RX_BUFFER_NUM; i++) {
            if (mcan_rx_get_buffer_status(&mcan_instance, i)) {
                rx_buffer_index = i;
                mcan_rx_clear_buffer_status(&mcan_instance, i);
                mcan_get_rx_buffer_element(&mcan_instance, &rx_element_buffer,
                rx_buffer_index);
                if (rx_element_buffer.R0.bit.XTD) {
                    printf("\n\r Extended message received in Rx buffer. The received data is: \r\n");
                    } else {
                    printf("\n\r Standard message received in Rx buffer. The received data is: \r\n");
                }
                for (i = 0; i < rx_element_buffer.R1.bit.DLC; i++) {
                    rx_element_buffer.data[i];
                    printf("  %d",rx_element_buffer.data[i]);
                    rx_element_buffer.data[i]=0;
                }
                printf("\r\n\r\n");
            }
        }
    }

    if (status & MCAN_RX_FIFO_0_NEW_MESSAGE) {
        mcan_clear_interrupt_status(&mcan_instance, MCAN_RX_FIFO_0_NEW_MESSAGE);
        mcan_get_rx_fifo_0_element(&mcan_instance, &rx_element_fifo_0,
        standard_receive_index);
        mcan_rx_fifo_acknowledge(&mcan_instance, 0,
        standard_receive_index);
        standard_receive_index++;
        if (standard_receive_index == CONF_MCAN0_RX_FIFO_0_NUM) {
            standard_receive_index = 0;
        }

        //printf("\n\r Standard message received in FIFO 0. The received data is: \r\n");
        for (i = 0; i < rx_element_fifo_0.R1.bit.DLC; i++) {
            //printf("  %d",rx_element_fifo_0.data[i]);
            CAN_data[i] = rx_element_fifo_0.data[i];
        }
        //printf("\r\n\r\n");
        CAN_data[0]*=1.85;
        CAN_data[3]*=4.7;
        
        /* Color prepare */
        if(CAN_data[1] < 90) {
            color_temp = GAR[0];
        }
        else if(CAN_data[1] < 100) {
            color_temp = GAR[1];
        }
        else {
            color_temp = GAR[2];
        }
        
        if(CAN_data[4]==0){
            color_gear = GAR[2];
        }
        else {
            color_gear = B;
        }
        
        SCB_CleanInvalidateDCache();
        //mcan_stop(&mcan_instance);
        //mcan_start(&mcan_instance);
    }
}


void can_test(int i) {
    if (i==0)
    mcan_send_standard_message(0x45A, tx_message_0, 8);
    else
    mcan_send_standard_message(0x469, tx_message_0, 4);
}

void can_init() {
    configure_mcan();
    //mcan_set_standard_filter_0();
    mcan_set_standard_filter_1();
}
