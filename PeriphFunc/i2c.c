/**
 * @file i2c.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief 
 * @version 1.02
 * @date 2022-07-19
 * 
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 * 
 */
#include "i2c.h"
#include "systick.h"
#include "utils.h"

#define OPTION_WRITE 0
#define OPTION_READ 1

static bool m_SoftI2cSclHigh(const SoftwareI2C *psI2c);
static bool m_SoftI2cSclLow(const SoftwareI2C *psI2c);
static bool m_SoftI2cSdaHigh(const SoftwareI2C *psI2c);
static bool m_SoftI2cSdaLow(const SoftwareI2C *psI2c);
static void m_SoftI2cDelay(const SoftwareI2C *psI2c);
static FlagStatus m_GetSoftI2cSda(const SoftwareI2C *psI2c);
static bool m_SoftI2cStart(const SoftwareI2C *psI2c);
static bool m_SoftI2cStop(const SoftwareI2C *psI2c);
static bool m_SoftI2cAck(const SoftwareI2C *psI2c);
static bool m_SoftI2cNAck(const SoftwareI2C *psI2c);
static bool m_SoftI2cWaitAck(const SoftwareI2C *psI2c);

static bool m_SoftI2cWriteByte(const SoftwareI2C *psI2c, uint8_t byte);
static uint8_t m_SoftI2cReadByte(const SoftwareI2C *psI2c);

static bool m_SoftI2cSendAddress(const SoftwareI2C *psI2c, uint8_t option,
								 uint16_t dev_addr, uint16_t reg_addr, uint8_t reg_addr_type);
static bool m_SoftI2cBurst(const SoftwareI2C *psI2c, uint8_t option,
						   uint8_t *pData, uint16_t size, uint32_t timeout);


void INewSoftwareI2C(const SoftwareI2C *psI2c)
{
	E_assert(psI2c != NULL);

	gpio_init(psI2c->sda_port, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, psI2c->sda_pin);
	gpio_init(psI2c->scl_port, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, psI2c->scl_pin);

	gpio_bit_write(psI2c->sda_port, psI2c->sda_pin, SET);
	gpio_bit_write(psI2c->scl_port, psI2c->scl_pin, SET);
}

static bool m_SoftI2cSclHigh(const SoftwareI2C *psI2c)
{
	gpio_bit_write(psI2c->scl_port, psI2c->scl_pin, SET);
	return true;
}

static bool m_SoftI2cSclLow(const SoftwareI2C *psI2c)
{
	gpio_bit_write(psI2c->scl_port, psI2c->scl_pin, RESET);
	return true;
}

static bool m_SoftI2cSdaHigh(const SoftwareI2C *psI2c)
{
	gpio_bit_write(psI2c->sda_port, psI2c->sda_pin, SET);
	return true;
}

static bool m_SoftI2cSdaLow(const SoftwareI2C *psI2c)
{
	gpio_bit_write(psI2c->sda_port, psI2c->sda_pin, RESET);
	return true;
}

#if 0 // not used because of the using rtos
void DelayUs(uint32_t nus)
{
    uint32_t temp = 0;
    if (nus == 0)
        return;
    SysTick->LOAD = nus * rcu_clock_freq_get(CK_SYS);       //时间加载
    SysTick->VAL = 0x00;                      //清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //开始倒数
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); //等待时间到达
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      //关闭计数器
    SysTick->VAL = 0x00;                            //清空计数器
}
#endif

void DelayUs(uint32_t nus)
{
	uint32_t ticks;
	uint32_t told, tnow, tcnt = 0;
	uint32_t reload = SysTick->LOAD;			//LOAD Value
	ticks = (SystemCoreClock / 1000000U) * nus; // The number of beats needed
	told = SysTick->VAL;						// Counter value at the time of first entry
	while (1)
	{

		tnow = SysTick->VAL;
		if (tnow != told)
		{

			if (tnow < told)
				tcnt += told - tnow; // Notice here SYSTICK It's a decreasing counter .
			else
				tcnt += reload - tnow + told;
			told = tnow;
			if (tcnt >= ticks)
				break; // For more than / Equal to the time to delay , The exit .
		}
	}
}

void DelayMs(uint32_t nms)
{
	while (nms--)
	{
		DelayUs(1000);
	}
}

static void m_SoftI2cDelay(const SoftwareI2C *psI2c)
{
	DelayUs(psI2c->delay_time);
}

static FlagStatus m_GetSoftI2cSda(const SoftwareI2C *psI2c)
{
	return gpio_input_bit_get(psI2c->sda_port, psI2c->sda_pin);
}

static bool m_SoftI2cStart(const SoftwareI2C *psI2c)
{
	bool tmpRst = true;

	tmpRst &= m_SoftI2cSdaHigh(psI2c);
	tmpRst &= m_SoftI2cSclHigh(psI2c);
	m_SoftI2cDelay(psI2c);
	if (RESET == m_GetSoftI2cSda(psI2c))
		return false;

	tmpRst &= m_SoftI2cSdaLow(psI2c);
	m_SoftI2cDelay(psI2c);
	if (SET == m_GetSoftI2cSda(psI2c))
		return false;

	return tmpRst;
}

static bool m_SoftI2cStop(const SoftwareI2C *psI2c)
{
	bool tmpRst = true;

	tmpRst &= m_SoftI2cSclLow(psI2c);
	m_SoftI2cDelay(psI2c);

	tmpRst &= m_SoftI2cSdaLow(psI2c);
	m_SoftI2cDelay(psI2c);

	tmpRst &= m_SoftI2cSclHigh(psI2c);
	m_SoftI2cDelay(psI2c);

	tmpRst &= m_SoftI2cSdaHigh(psI2c);
	m_SoftI2cDelay(psI2c);

	return tmpRst;
}

static bool m_SoftI2cAck(const SoftwareI2C *psI2c)
{
	bool tmpRst = true;

	tmpRst &= m_SoftI2cSclLow(psI2c);
	m_SoftI2cDelay(psI2c);

	tmpRst &= m_SoftI2cSdaLow(psI2c);
	m_SoftI2cDelay(psI2c);

	tmpRst &= m_SoftI2cSclHigh(psI2c);
	m_SoftI2cDelay(psI2c);

	tmpRst &= m_SoftI2cSclLow(psI2c);
	m_SoftI2cDelay(psI2c);

	return tmpRst;
}

static bool m_SoftI2cNAck(const SoftwareI2C *psI2c)
{
	bool tmpRst = true;

	tmpRst &= m_SoftI2cSclLow(psI2c);
	m_SoftI2cDelay(psI2c);

	tmpRst &= m_SoftI2cSdaHigh(psI2c);
	m_SoftI2cDelay(psI2c);

	tmpRst &= m_SoftI2cSclHigh(psI2c);
	m_SoftI2cDelay(psI2c);

	tmpRst &= m_SoftI2cSclLow(psI2c);
	m_SoftI2cDelay(psI2c);

	return tmpRst;
}

static bool m_SoftI2cWaitAck(const SoftwareI2C *psI2c)
{
	uint8_t i = 0;

	m_SoftI2cSclLow(psI2c);
	m_SoftI2cDelay(psI2c);

	m_SoftI2cSdaHigh(psI2c);
	m_SoftI2cDelay(psI2c);

	m_SoftI2cSclHigh(psI2c);
	m_SoftI2cDelay(psI2c);

	while (SET == m_GetSoftI2cSda(psI2c))
	{
		i++;
		if (i >= 250)
		{
			m_SoftI2cStop(psI2c);
			EXCUTE_ONCE(printf("slave no ack\n"));
			return false;
		}
	}

	m_SoftI2cSclLow(psI2c);

	return true;
}

/**
 * @brief The transmission byte order is the same as the CPU memory byte order. 
 * Gd32 is a little endian, and the low order is first and the high order is last.
 * 
 * @param psI2c iic device
 * @param dev_addr seherical device address
 * @param reg_addr register address
 * @param reg_addr_type register address type
 * @param pData data pointer
 * @param size data size
 * @param timeout transfer timeout
 * @return bool 
 */
bool ISoftwareI2CRegWrite(const SoftwareI2C *psI2c, uint16_t dev_addr, uint16_t reg_addr, uint8_t reg_addr_type, uint8_t *pData, uint16_t size, uint32_t timeout)
{
	bool tempRet = true;

	/* Start I2C. */
	tempRet &= m_SoftI2cStart(psI2c);
	if (!tempRet) 
		return tempRet;

	/* WRITE: Send Device Address that its last bit must be 0.*/
	tempRet &= m_SoftI2cSendAddress(psI2c, OPTION_WRITE, dev_addr, reg_addr, reg_addr_type);
	if (!tempRet) 
		return tempRet;

	/* Send data. */
	tempRet &= m_SoftI2cBurst(psI2c, OPTION_WRITE, pData, size, timeout);
	if (!tempRet) 
		return tempRet;

	/* Stop Writing. */
	tempRet &= m_SoftI2cStop(psI2c);

	return tempRet;
}

bool ISoftwareI2CRegRead(const SoftwareI2C *psI2c, uint16_t dev_addr, uint16_t reg_addr, uint8_t reg_addr_type, uint8_t *pData, uint16_t size, uint32_t timeout)
{

	bool tempRet = true;

	/* Start I2C. */
	tempRet &= m_SoftI2cStart(psI2c);
	if (!tempRet) 
		return tempRet;

	/* READ: Send Device Address that its last bit must be 1. */
	tempRet &= m_SoftI2cSendAddress(psI2c, OPTION_READ, dev_addr, reg_addr, reg_addr_type);
	if (!tempRet) 
		return tempRet;

	/* Receve Data. */
	tempRet &= m_SoftI2cBurst(psI2c, OPTION_READ, pData, size, timeout);
	if (!tempRet) 
		return tempRet;

	/* Stop Reading. */
	tempRet &= m_SoftI2cStop(psI2c);

	return tempRet;
}

static bool m_SoftI2cWriteByte(const SoftwareI2C *psI2c, uint8_t byte)
{
	uint8_t i = 8;
	bool tmpRst = true;

	while (i--)
	{
		tmpRst &= m_SoftI2cSclLow(psI2c);
		m_SoftI2cDelay(psI2c);

		if ((byte & 0x80))
		{
			tmpRst &= m_SoftI2cSdaHigh(psI2c);
		}
		else
		{
			tmpRst &= m_SoftI2cSdaLow(psI2c);
		}

		byte = byte << 1;
		m_SoftI2cDelay(psI2c);
		tmpRst &= m_SoftI2cSclHigh(psI2c);
		m_SoftI2cDelay(psI2c);
	}
	return tmpRst;
}

static uint8_t m_SoftI2cReadByte(const SoftwareI2C *psI2c)
{
	uint8_t i = 8;
	uint8_t byte = 0;

	m_SoftI2cSdaHigh(psI2c);
	while (i--)
	{
		byte = byte << 1;

		m_SoftI2cSclLow(psI2c);
		m_SoftI2cDelay(psI2c);
		m_SoftI2cSclHigh(psI2c);
		m_SoftI2cDelay(psI2c);

		if (m_GetSoftI2cSda(psI2c))
			byte |= 0x01;
	}
	m_SoftI2cSclLow(psI2c);

	return byte;
}

static bool m_SoftI2cSendAddress(const SoftwareI2C *psI2c, uint8_t option,
								 uint16_t dev_addr, uint16_t reg_addr, uint8_t reg_addr_type)
{
	bool stat = true;
	/* First Write the Device Address and send the "WRITE CMD"
     * by setting the last bit 0. */
	stat &= m_SoftI2cWriteByte(psI2c, dev_addr & 0xFE);
	stat &= m_SoftI2cWaitAck(psI2c);
	if (!stat)
		return stat;

	/* Send Register Address. */
	if (REG_ADDR_1BYTE == reg_addr_type)
	{
		stat &= m_SoftI2cWriteByte(psI2c, reg_addr & 0xFF);
		stat &= m_SoftI2cWaitAck(psI2c);
		if (!stat)
			return stat;
	}
	else if (REG_ADDR_2BYTE == reg_addr_type)
	{
		stat &= m_SoftI2cWriteByte(psI2c, (reg_addr >> 8) & 0xFF);
		stat &= m_SoftI2cWaitAck(psI2c);
		if(!stat)
			return stat;

		stat &= m_SoftI2cWriteByte(psI2c, reg_addr & 0xFF);
		stat &= m_SoftI2cWaitAck(psI2c);
		if(!stat)
			return stat;
	}

	/* If option = READ, Device Address need to be sent again with "READ CMD"
	 * by setting the last bit 1.
     */
	if (OPTION_READ == option)
	{
		stat &= m_SoftI2cStop(psI2c);
		stat &= m_SoftI2cStart(psI2c);

		stat &= m_SoftI2cWriteByte(psI2c, dev_addr | 0x01);
		stat &= m_SoftI2cWaitAck(psI2c);
		if(!stat)
			return stat;
	}

	return stat;
}

static bool m_SoftI2cBurst(const SoftwareI2C *psI2c, uint8_t option,
						   uint8_t *pData, uint16_t size, uint32_t timeout)
{
	bool stat = true;

	if (OPTION_READ == option)
	{
		/* Receve Data. */
		while (size--)
		{
			*pData = m_SoftI2cReadByte(psI2c);
			if (size)
			{
				stat &= m_SoftI2cAck(psI2c);
			}
			else
			{
				stat &= m_SoftI2cNAck(psI2c);
			}
			pData++;
		}
	}
	else if (OPTION_WRITE == option)
	{
		/* Send data. */
		while (size--)
		{
			stat &= m_SoftI2cWriteByte(psI2c, *pData);
			stat &= m_SoftI2cWaitAck(psI2c);
			if(!stat)
				return stat;
			pData++;
		}
	}

	return stat;
}
