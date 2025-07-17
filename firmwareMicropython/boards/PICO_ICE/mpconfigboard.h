#ifdef PICO_ICE
#define MICROPY_HW_BOARD_NAME "tinyVision.ai pico-ice"
#elifdef PICO2_ICE
#define MICROPY_HW_BOARD_NAME "tinyVision.ai pico2-ice"
#endif

// Allow 1MB for the firmware image itself, allocate the remainder to the filesystem
#define MICROPY_HW_FLASH_STORAGE_BYTES (PICO_FLASH_SIZE_BYTES - (1 * 1024 * 1024))

#define MICROPY_HW_USB_MSC		(1)

#define MICROPY_HW_USB_VID		(0x1209)
#define MICROPY_HW_USB_PID		(0xB1C0)


/* Micropython default pin definitions */
#ifdef PICO_ICE
/* FPGA UART */
#define MICROPY_HW_UART0_TX		(0)
#define MICROPY_HW_UART0_RX		(1)
#define MICROPY_HW_UART0_CTS	(2)
#define MICROPY_HW_UART0_RTS	(3)
/* FPGA SPI */
#define MICROPY_HW_SPI1_SCK		(10)
#define MICROPY_HW_SPI1_MOSI	(11)
#define MICROPY_HW_SPI1_MISO	(8)
/* PMOD SPI */
#define MICROPY_HW_SPI0_SCK 	(18)
#define MICROPY_HW_SPI0_MOSI	(19)
#define MICROPY_HW_SPI0_MISO	(16)
/* i2C0 */
#define MICROPY_HW_I2C0_SCL		(13)
#define MICROPY_HW_I2C0_SDA		(12)

#elifdef PICO2_ICE
/* FPGA SPI */
#define MICROPY_HW_SPI0_SCK		(6)
#define MICROPY_HW_SPI0_MOSI	(7)
#define MICROPY_HW_SPI0_MISO	(4)
/* PMOD SPI */
#define MICROPY_HW_SPI1_SCK		(34)
#define MICROPY_HW_SPI1_MOSI	(35)
#define MICROPY_HW_SPI1_MISO	(32)
/* I2C1 on header */
#define MICROPY_HW_I2C1_SCL		(3)
#define MICROPY_HW_I2C1_SDA		(2)

#define MICROPY_HW_PSRAM_CS_PIN	(8)
#define MICROPY_HW_ENABLE_PSRAM	(1)
#endif
