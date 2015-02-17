#ifndef A7105_CONFIG_H_INCLUDED
#define A7105_CONFIG_H_INCLUDED

#define WTR_DDR DDRA
#define WTR_PIN PIN7
#define WTR_PORT PINA

#define CS_DDR DDRA
#define CS_PIN PIN4
#define CS_PORT PORTA

// GIO1 como pin WTR
#define GIO1 0x01
// GIO1 como pin MISO
//#define GIO1 0x19

// GIO2 desactivado
#define GIO2 0x00
// GIO2 como pin WTR
//#define GIO2 0x01

#define FL 7 // largo fifo

#define ID0 0xAA
#define ID1 0xAB
#define ID2 0xAC
#define ID3 0xAD

// activar encriptacion
#define EN_DATA_WHITENING
#define CD3R 0xAA // Semilla para encriptacion (XOR)
#define EN_FEC
#define EN_CRC
#define IDL_4
#define PML 0x04

//#define DATA_RATE_500K
//#define DATA_RATE_250K
//#define DATA_RATE_125K
#define DATA_RATE_50K
//#define DATA_RATE_25K
//#define DATA_RATE_10K
//#define DATA_RATE_2K

/******************************************************/
/*      Modificar con cuidado                         */
/******************************************************/

#ifdef EN_DATA_WHITENING
#define _DATA_WHITENING 0x20
#else
#define _DATA_WHITENING 0x00
#endif

#ifdef EN_FEC
#define _FEC 0x10
#else
#define _FEC 0x00
#endif // EN_FEC

#ifdef EN_CRC
#define _CRC 0x08
#else
#define _CRC 0x00
#endif // EN_CRC

#ifdef IDL_2
#define _IDL 0x00
#endif // IDL_2

#ifdef IDL_4
#define _IDL 0x04
#endif

#ifdef DATA_RATE_500K
#define DTRR 0x00
#endif

#ifdef DATA_RATE_250K
#define DTRR 0x01
#endif // DATA_RATE_250K

#ifdef DATA_RATE_125K
#define DTRR 0x03
#endif // DATA_RATE_125K

#ifdef DATA_RATE_100K
#define DTRR 0x04
#endif // DATA_RATE_100K

#ifdef DATA_RATE_50K
#define DTRR 0x09
#endif // DATA_RATE_50K

#ifdef DATA_RATE_25K
#define DTRR 0x13
#endif // DATA_RATE_25K

#ifdef DATA_RATE_10K
#define DTRR 0x31
#endif // DATA_RATE_10K

#ifdef DATA_RATE_2K
#define DTRR 0xF9
#endif // DATA_RATE_2K

// va al registro CODE1 (0x20)
#define CD1R (_DATA_WHITENING | _FEC | _CRC | _IDL | ( (PML-1) & 0x02) )

#ifndef CD3R
#define CD3R 0xAA
#endif

const uint8_t A7105_config[] = {
    0x00, // (0x00) RESET register
    //0x42, // (0x01) MODE register
    0x62, // (0x01) MODE register
    0x00, // (0x02) Calibration registerr
    FL-1, // (0x03) FIFO1 register (64 posiciones)
    0x00, // (0x04) FIFO2 register
    0x00, // (0x05) FIFO register
    0x00, // (0x06) IDDATA register
    0x00, // (0x07) RCOSC1 register (reservado)
    0x00, // (0x08) RCOSC2 register (reservado)
    0x00, // (0x09) RCOSC3 register
    0x00, // (0x0A) CKO register (pin no disponible)
    //0x19, // (0x0B) GIO1 register (pin MISO)
    GIO1, // (0x0B) GIO1 register (pin WTR)
    GIO2, // (0x0C) GIO2 register
    0x05, // (0x0D) CLOCK register (CSC = 01 (FSYSCK=FMCLK/2), XS = 1)
    DTRR, // (0x0E) DATARATE register
    0x01, // (0x0F) PLL1 register (Canal 1)
    0x9E, // (0x10) PLL2 register (DBL = 1, CHR = 15)
    0x4B, // (0x11) PLL3 register (BIP = 0x4B)
    0x00, // (0x12) PLL4 register (BFP_H = 0x00)
    0x02, // (0x13) PLL5 register (BFP_L = 0x02)
    0x16, // (0x14) TX1 register (Activar modulacion TX)
    0x2B, // (0x15) TX2 register
    0x12, // (0x16) DELAY1 register
    0x40, // (0x17) DELAY2 register (WSEL=0b010)
    0x62, // (0x18) RX register
    0x80, // (0x19) RXGAIN1 register
    0x80, // (0x1A) RXGAIN2 register (Reservado)
    0x00, // (0x1B) RXGAIN3 register (Reservado)
    0x0A, // (0x1C) RXGAIN4 register (Reservado, escribir)
    0x32, // (0x1D) RSSI register
    0xC3, // (0x1E) ADC register (leer RSSI, continuo)
    //0x07, // (0x1F) CODE1 register (Sin encriptacion, sin CRC, ID=4bytes, preambulo 4bytes)
    //0x1F, // (0x1F) CODE1 register (Sin encriptacion, con FEC, con CRC, ID=4bytes, preambulo 4bytes)
    //0x1F, // (0x1F) CODE1 register (con encriptacion, con FEC, con CRC, ID=4bytes, preambulo 4bytes)
    CD1R, // (0x1F) CODE1 register
    0x16, // (0x20) CODE2 register
    CD3R, // (0x21) CODE3 register (semillas para encriptacion, solo se usa si hay encriptacion)
    0x00, // (0x22) IFCAL1 register (calibracion auto)
    0x00, // (0x23) IFCAL2 register (IF filter cal deviation from goal, read only)
    0x13, // (0x24) VCOCCAL register
    0x00, // (0x25) VCOCAL1 register
    0x3B, // (0x26) VCOCAL2 register
    0x00, // (0x27) BATTERY register
    0x1F, // (0x28) TXTEST register (output power 1dBm)
    0x47, // (0x29) RXDEM1 register
    0x80, // (0x2A) RXDEM2 register (valor recomendado)
    0x03, // (0x2B) CPC register (charge pump current, 2mA)
    0x01, // (0x2C) CRYSTAL register (reservado, escribir)
    0x45, // (0x2D) PLLTEST register (reservado, escribir)
    0x18, // (0x2E) VCOTEST1 register (reservado, escribir)
    0x00, // (0x2F) VCOTEST2 register (valor recomendado 0x00)
    0x01, // (0x30) IFAT register (reservado, escribir)
    0x0F, // (0x31) RSCALE register (reservado, escribir)
    0x00, // (0x32) FILTERTEST register (reservado, escribir)
};


#endif // A7105_CONFIG_H_INCLUDED
