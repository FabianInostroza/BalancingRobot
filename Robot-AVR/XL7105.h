#ifndef XL71905_H_INCLUDED
#define XL71905_H_INCLUDED

typedef enum{SLEEP_MODE = 0x80,
             IDLE_MODE = 0x90,
             STANDBY_MODE = 0xA0,
             PLL_MODE = 0xB0,
             RX_MODE = 0xC0,
             TX_MODE = 0xD0,
             FIFO_WP_R = 0xE0,
             FIFO_RP_R = 0xF0,
}STROBE_CMD;

uint8_t setup_xl7105(void);
uint8_t xl7105_read(uint8_t reg);
void xl7105_write(uint8_t reg, uint8_t data);
void xl7105_strobe_cmd(STROBE_CMD cmd);
void xl7105_write_id(uint8_t id0, uint8_t id1, uint8_t id2, uint8_t id3);
void xl7105_read_id(uint8_t * id);
uint8_t xl7105_if_calibration(void);
uint8_t xl7105_vco_calibration(void);
uint8_t xl7105_rx(uint8_t * data, uint8_t n, uint8_t cont);
uint8_t xl7105_tx(uint8_t * data, uint8_t n, uint8_t block);
inline uint8_t xl7105_rssi(void);

#endif // XL71905_H_INCLUDED
