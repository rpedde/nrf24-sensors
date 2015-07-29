/*
 * nrf24.h Copyright (C) 2015 Ron Pedde <ron@pedde.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __NRF24_H__
#define __NRF24_H__

#define NRF_CE_DDR  DDRD
#define NRF_CE_PORT PORTD
#define NRF_CE_PIN  5

/* Register Map */
#define REG_CONFIG        0x00  /* config register */
#define REG_EN_AA         0x01  /* enable auto_acknowledgement */
#define REG_EN_RXADDR     0x02  /* enabled rx addresses */
#define REG_SETUP_AW      0x03  /* setup address widths */
#define REG_SETUP_RETR    0x04  /* setup auto retransmission */
#define REG_RF_CH         0x05  /* rf channel */
#define REG_RF_SETUP      0x06  /* rf setup register */
#define REG_STATUS        0x07  /* status register */
#define REG_OBSERVE_TX    0x08  /* transmit observe register */
#define REG_RPD           0x09  /* receive power detector */
#define REG_RX_ADDR_P0    0x0a  /* receive address data pipe 0 */
#define REG_RX_ADDR_P1    0x0b  /* receive address data pipe 1 */
#define REG_RX_ADDR_P2    0x0c  /* receive address data pipe 2 */
#define REG_RX_ADDR_P3    0x0d  /* receive address data pipe 3 */
#define REG_RX_ADDR_P4    0x0e  /* receive address data pipe 4 */
#define REG_RX_ADDR_P5    0x0f  /* receive address data pipe 5 */
#define REG_TX_ADDR       0x10  /* transmit address */
#define REG_RX_PW_P0      0x11  /* bytes in rx payload in pipe 0 */
#define REG_RX_PW_P1      0x12  /* bytes in rx payload in pipe 1 */
#define REG_RX_PW_P2      0x13  /* bytes in rx payload in pipe 2 */
#define REG_RX_PW_P3      0x14  /* bytes in rx payload in pipe 3 */
#define REG_RX_PW_P4      0x15  /* bytes in rx payload in pipe 4 */
#define REG_RX_PW_P5      0x16  /* bytes in rx payload in pipe 5 */
#define REG_FIFO_STATUS   0x17  /* fifo status register */
#define REG_DYNPD         0x1c  /* enable dynamic payload length */

#define REG_READ          0x00  /* read a register */
#define REG_WRITE         0x20  /* write a register */

#define REG_READ_RX       0x61  /* read the rx payload */
#define REG_WRITE_TX      0xA0  /* write the tx payload */

#define REG_FLUSH_RX      0xE2  /* flush the rx buffer */
#define REG_FLUSH_TX      0xE1  /* flush the tx buffer */

#define REG_NOP           0xFF  /* non-action byte to receive data */


/* bitfields */
#define REG_CONFIG_MASK_RX_DR     6
#define REG_CONFIG_MASK_TX_DR     5
#define REG_CONFIG_MASK_MAX_RT    4
#define REG_CONFIG_EN_CRC         3
#define REG_CONFIG_EN_CRCO        2
#define REG_CONFIG_EN_PWR_UP      1
#define REG_CONFIG_EN_PRIM_RX     0

#define REG_STATUS_RX_DR          6
#define REG_STATUS_TX_DS          5
#define REG_STATUS_MAX_RT         4
#define REG_STATUS_TX_FULL        0

#define REG_RXADDR_ERX_P5         5
#define REG_RXADDR_ERX_P4         4
#define REG_RXADDR_ERX_P3         3
#define REG_RXADDR_ERX_P2         2
#define REG_RXADDR_ERX_P1         1
#define REG_RXADDR_ERX_P0         0

extern void nrf24_init(void);
extern void nrf24_enable(int enable);
extern void nrf24_read_write_vector_register(uint8_t direction,
                                             uint8_t reg,
                                             uint8_t *buffer,
                                             uint8_t len);
extern uint8_t nrf24_read_register(uint8_t reg);
extern void nrf24_write_register(uint8_t reg, uint8_t value);
extern void nrf24_power_up(int powerup);
extern void nrf24_config_tx(uint8_t *tx_addr, uint8_t *rx_addr);
extern void nrf24_config_rx(uint8_t *rx_addr);
extern void nrf24_transmit(uint8_t *buf, uint8_t len);
extern int nrf24_receive(uint8_t *buf, uint8_t len, uint8_t pipe);
extern void nrf24_reset_irq(void);


#endif /* __NRF24_H__ */
