

#include "drv_gpio.h"

#define USING_PIN


#ifdef USING_PIN

static const struct pin_index pins[] = {
#if defined(GPIOA)
	__STM32_PIN(0,  A, 0 ),
	__STM32_PIN(1,  A, 1 ),
	__STM32_PIN(2,  A, 2 ),
	__STM32_PIN(3,  A, 3 ),
	__STM32_PIN(4,  A, 4 ),
	__STM32_PIN(5,  A, 5 ),
	__STM32_PIN(6,  A, 6 ),
	__STM32_PIN(7,  A, 7 ),
	__STM32_PIN(8,  A, 8 ),
	__STM32_PIN(9,  A, 9 ),
	__STM32_PIN(10,  A, 10),
	__STM32_PIN(11,  A, 11),
	__STM32_PIN(12,  A, 12),
	__STM32_PIN(13,  A, 13),
	__STM32_PIN(14,  A, 14),
	__STM32_PIN(15,  A, 15),
#if defined(GPIOB)
	__STM32_PIN(16,  B, 0),
	__STM32_PIN(17,  B, 1),
	__STM32_PIN(18,  B, 2),
	__STM32_PIN(19,  B, 3),
	__STM32_PIN(20,  B, 4),
	__STM32_PIN(21,  B, 5),
	__STM32_PIN(22,  B, 6),
	__STM32_PIN(23,  B, 7),
	__STM32_PIN(24,  B, 8),
	__STM32_PIN(25,  B, 9),
	__STM32_PIN(26,  B, 10),
	__STM32_PIN(27,  B, 11),
	__STM32_PIN(28,  B, 12),
	__STM32_PIN(29,  B, 13),
	__STM32_PIN(30,  B, 14),
	__STM32_PIN(31,  B, 15),
#if defined(GPIOC)
	__STM32_PIN(32,  C, 0),
	__STM32_PIN(33,  C, 1),
	__STM32_PIN(34,  C, 2),
	__STM32_PIN(35,  C, 3),
	__STM32_PIN(36,  C, 4),
	__STM32_PIN(37,  C, 5),
	__STM32_PIN(38,  C, 6),
	__STM32_PIN(39,  C, 7),
	__STM32_PIN(40,  C, 8),
	__STM32_PIN(41,  C, 9),
	__STM32_PIN(42,  C, 10),
	__STM32_PIN(43,  C, 11),
	__STM32_PIN(44,  C, 12),
	__STM32_PIN(45,  C, 13),
	__STM32_PIN(46,  C, 14),
	__STM32_PIN(47,  C, 15),
#if defined(GPIOD)
	__STM32_PIN(48,  D, 0),
	__STM32_PIN(49,  D, 1),
	__STM32_PIN(50,  D, 2),
	__STM32_PIN(51,  D, 3),
	__STM32_PIN(52,  D, 4),
	__STM32_PIN(53,  D, 5),
	__STM32_PIN(54,  D, 6),
	__STM32_PIN(55,  D, 7),
	__STM32_PIN(56,  D, 8),
	__STM32_PIN(57,  D, 9),
	__STM32_PIN(58,  D, 10),
	__STM32_PIN(59,  D, 11),
	__STM32_PIN(60,  D, 12),
	__STM32_PIN(61,  D, 13),
	__STM32_PIN(62,  D, 14),
	__STM32_PIN(63,  D, 15),
#if defined(GPIOE)
	__STM32_PIN(64,  E, 0),
	__STM32_PIN(65,  E, 1),
	__STM32_PIN(66,  E, 2),
	__STM32_PIN(67,  E, 3),
	__STM32_PIN(68,  E, 4),
	__STM32_PIN(69,  E, 5),
	__STM32_PIN(70,  E, 6),
	__STM32_PIN(71,  E, 7),
	__STM32_PIN(72,  E, 8),
	__STM32_PIN(73,  E, 9),
	__STM32_PIN(74,  E, 10),
	__STM32_PIN(75,  E, 11),
	__STM32_PIN(76,  E, 12),
	__STM32_PIN(77,  E, 13),
	__STM32_PIN(78,  E, 14),
	__STM32_PIN(79,  E, 15),
#if defined(GPIOF)
	__STM32_PIN(80,  F, 0),
	__STM32_PIN(81,  F, 1),
	__STM32_PIN(82,  F, 2),
	__STM32_PIN(83,  F, 3),
	__STM32_PIN(84,  F, 4),
	__STM32_PIN(85,  F, 5),
	__STM32_PIN(86,  F, 6),
	__STM32_PIN(87,  F, 7),
	__STM32_PIN(88,  F, 8),
	__STM32_PIN(89,  F, 9),
	__STM32_PIN(90,  F, 10),
	__STM32_PIN(91,  F, 11),
	__STM32_PIN(92,  F, 12),
	__STM32_PIN(93,  F, 13),
	__STM32_PIN(94,  F, 14),
	__STM32_PIN(95,  F, 15),
#if defined(GPIOG)
	__STM32_PIN(96,  G, 0),
	__STM32_PIN(97,  G, 1),
	__STM32_PIN(98,  G, 2),
	__STM32_PIN(99,  G, 3),
	__STM32_PIN(100, G, 4),
	__STM32_PIN(101, G, 5),
	__STM32_PIN(102, G, 6),
	__STM32_PIN(103, G, 7),
	__STM32_PIN(104, G, 8),
	__STM32_PIN(105, G, 9),
	__STM32_PIN(106, G, 10),
	__STM32_PIN(107, G, 11),
	__STM32_PIN(108, G, 12),
	__STM32_PIN(109, G, 13),
	__STM32_PIN(110, G, 14),
	__STM32_PIN(111, G, 15),
#if defined(GPIOH)
	__STM32_PIN(112, H, 0),
	__STM32_PIN(113, H, 1),
	__STM32_PIN(114, H, 2),
	__STM32_PIN(115, H, 3),
	__STM32_PIN(116, H, 4),
	__STM32_PIN(117, H, 5),
	__STM32_PIN(118, H, 6),
	__STM32_PIN(119, H, 7),
	__STM32_PIN(120, H, 8),
	__STM32_PIN(121, H, 9),
	__STM32_PIN(122, H, 10),
	__STM32_PIN(123, H, 11),
	__STM32_PIN(124, H, 12),
	__STM32_PIN(125, H, 13),
	__STM32_PIN(126, H, 14),
	__STM32_PIN(127, H, 15),
#if defined(GPIOI)
	__STM32_PIN(128, I, 0),
	__STM32_PIN(129, I, 1),
	__STM32_PIN(130, I, 2),
	__STM32_PIN(131, I, 3),
	__STM32_PIN(132, I, 4),
	__STM32_PIN(133, I, 5),
	__STM32_PIN(134, I, 6),
	__STM32_PIN(135, I, 7),
	__STM32_PIN(136, I, 8),
	__STM32_PIN(137, I, 9),
	__STM32_PIN(138, I, 10),
	__STM32_PIN(139, I, 11),
	__STM32_PIN(140, I, 12),
	__STM32_PIN(141, I, 13),
	__STM32_PIN(142, I, 14),
	__STM32_PIN(143, I, 15),
#if defined(GPIOJ)
	__STM32_PIN(144, J, 0),
	__STM32_PIN(145, J, 1),
	__STM32_PIN(146, J, 2),
	__STM32_PIN(147, J, 3),
	__STM32_PIN(148, J, 4),
	__STM32_PIN(149, J, 5),
	__STM32_PIN(150, J, 6),
	__STM32_PIN(151, J, 7),
	__STM32_PIN(152, J, 8),
	__STM32_PIN(153, J, 9),
	__STM32_PIN(154, J, 10),
	__STM32_PIN(155, J, 11),
	__STM32_PIN(156, J, 12),
	__STM32_PIN(157, J, 13),
	__STM32_PIN(158, J, 14),
	__STM32_PIN(159, J, 15),
#if defined(GPIOK)
	__STM32_PIN(160, K, 0),
	__STM32_PIN(161, K, 1),
	__STM32_PIN(162, K, 2),
	__STM32_PIN(163, K, 3),
	__STM32_PIN(164, K, 4),
	__STM32_PIN(165, K, 5),
	__STM32_PIN(166, K, 6),
	__STM32_PIN(167, K, 7),
	__STM32_PIN(168, K, 8),
	__STM32_PIN(169, K, 9),
	__STM32_PIN(170, K, 10),
	__STM32_PIN(171, K, 11),
	__STM32_PIN(172, K, 12),
	__STM32_PIN(173, K, 13),
	__STM32_PIN(174, K, 14),
	__STM32_PIN(175, K, 15),
#if defined(GPIOZ)
	__STM32_PIN(176, Z, 0),
	__STM32_PIN(177, Z, 1),
	__STM32_PIN(178, Z, 2),
	__STM32_PIN(179, Z, 3),
	__STM32_PIN(180, Z, 4),
	__STM32_PIN(181, Z, 5),
	__STM32_PIN(182, Z, 6),
	__STM32_PIN(183, Z, 7),
	__STM32_PIN(184, Z, 8),
	__STM32_PIN(185, Z, 9),
	__STM32_PIN(186, Z, 10),
	__STM32_PIN(187, Z, 11),
	__STM32_PIN(188, Z, 12),
	__STM32_PIN(189, Z, 13),
	__STM32_PIN(190, Z, 14),
	__STM32_PIN(191, Z, 15),
#endif /* defined(GPIOZ) */
#endif /* defined(GPIOK) */
#endif /* defined(GPIOJ) */
#endif /* defined(GPIOI) */
#endif /* defined(GPIOH) */
#endif /* defined(GPIOG) */
#endif /* defined(GPIOF) */
#endif /* defined(GPIOE) */
#endif /* defined(GPIOD) */
#endif /* defined(GPIOC) */
#endif /* defined(GPIOB) */
#endif /* defined(GPIOA) */
};

static const struct pin_irq_map pin_irq_map[] = {
#if defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32G0)
	{GPIO_PIN_0, EXTI0_1_IRQn},
	{GPIO_PIN_1, EXTI0_1_IRQn},
	{GPIO_PIN_2, EXTI2_3_IRQn},
	{GPIO_PIN_3, EXTI2_3_IRQn},
	{GPIO_PIN_4, EXTI4_15_IRQn},
	{GPIO_PIN_5, EXTI4_15_IRQn},
	{GPIO_PIN_6, EXTI4_15_IRQn},
	{GPIO_PIN_7, EXTI4_15_IRQn},
	{GPIO_PIN_8, EXTI4_15_IRQn},
	{GPIO_PIN_9, EXTI4_15_IRQn},
	{GPIO_PIN_10, EXTI4_15_IRQn},
	{GPIO_PIN_11, EXTI4_15_IRQn},
	{GPIO_PIN_12, EXTI4_15_IRQn},
	{GPIO_PIN_13, EXTI4_15_IRQn},
	{GPIO_PIN_14, EXTI4_15_IRQn},
	{GPIO_PIN_15, EXTI4_15_IRQn},
#elif defined(SOC_SERIES_STM32MP1)
	{GPIO_PIN_0, EXTI0_IRQn},
	{GPIO_PIN_1, EXTI1_IRQn},
	{GPIO_PIN_2, EXTI2_IRQn},
	{GPIO_PIN_3, EXTI3_IRQn},
	{GPIO_PIN_4, EXTI4_IRQn},
	{GPIO_PIN_5, EXTI5_IRQn},
	{GPIO_PIN_6, EXTI6_IRQn},
	{GPIO_PIN_7, EXTI7_IRQn},
	{GPIO_PIN_8, EXTI8_IRQn},
	{GPIO_PIN_9, EXTI9_IRQn},
	{GPIO_PIN_10, EXTI10_IRQn},
	{GPIO_PIN_11, EXTI11_IRQn},
	{GPIO_PIN_12, EXTI12_IRQn},
	{GPIO_PIN_13, EXTI13_IRQn},
	{GPIO_PIN_14, EXTI14_IRQn},
	{GPIO_PIN_15, EXTI15_IRQn},
#else
	{GPIO_PIN_0, EXTI0_IRQn},
	{GPIO_PIN_1, EXTI1_IRQn},
	{GPIO_PIN_2, EXTI2_IRQn},
	{GPIO_PIN_3, EXTI3_IRQn},
	{GPIO_PIN_4, EXTI4_IRQn},
	{GPIO_PIN_5, EXTI9_5_IRQn},
	{GPIO_PIN_6, EXTI9_5_IRQn},
	{GPIO_PIN_7, EXTI9_5_IRQn},
	{GPIO_PIN_8, EXTI9_5_IRQn},
	{GPIO_PIN_9, EXTI9_5_IRQn},
	{GPIO_PIN_10, EXTI15_10_IRQn},
	{GPIO_PIN_11, EXTI15_10_IRQn},
	{GPIO_PIN_12, EXTI15_10_IRQn},
	{GPIO_PIN_13, EXTI15_10_IRQn},
	{GPIO_PIN_14, EXTI15_10_IRQn},
	{GPIO_PIN_15, EXTI15_10_IRQn},
#endif
};

static struct pin_irq_hdr pin_irq_hdr_tab[] = {
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
	{-1, 0, NULL, NULL},
};
static uint32_t pin_irq_enable_mask = 0;

#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])

static const struct pin_index *get_pin(uint8_t pin)
{
	const struct pin_index *index;

	if (pin < ITEM_NUM(pins)) {
		index = &pins[pin];
		if (index->index == -1)
			index = RT_NULL;
	} else {
		index = RT_NULL;
	}

	return index;
};

static rt_base_t stm32_pin_get(const char *name)
{
	rt_base_t pin = 0;
	int hw_port_num, hw_pin_num = 0;
	int i, name_len = 1;
	int mul = 1;

	name_len = rt_strlen(name);

	if ((name_len < 4) || (name_len >= 6)) {
		return -RT_EINVAL;
	}
	if ((name[0] != 'P') || (name[2] != '.')) {
		return -RT_EINVAL;
	}

	if ((name[1] >= 'A') && (name[1] <= 'Z')) {
		hw_port_num = (int)(name[1] - 'A');
	} else {
		return -RT_EINVAL;
	}

	for (i = name_len - 1; i > 2; i--) {
		hw_pin_num += ((int)(name[i] - '0') * mul);
		mul = mul * 10;
	}

	pin = 16 * hw_port_num + hw_pin_num;

	if (pin < ITEM_NUM(pins)) {
		return pin;
	} else {
		return -RT_EINVAL;
	}
}
static void stm32_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
	const struct pin_index *index;

	index = get_pin(pin);
	if (index == RT_NULL) {
		return;
	}

	HAL_GPIO_WritePin(index->gpio, index->pin, (GPIO_PinState)value);
}

static int stm32_pin_read(rt_device_t dev, rt_base_t pin)
{
	int value;
	const struct pin_index *index;

	value = PIN_LOW;

	index = get_pin(pin);
	if (index == RT_NULL) {
		return value;
	}

	value = HAL_GPIO_ReadPin(index->gpio, index->pin);

	return value;
}

static void stm32_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
	const struct pin_index *index;
	GPIO_InitTypeDef GPIO_InitStruct;

	index = get_pin(pin);
	if (index == RT_NULL) {
		return;
	}

	/* Configure GPIO_InitStructure */
	GPIO_InitStruct.Pin = index->pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	if (mode == PIN_MODE_OUTPUT) {
		/* output setting */
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	} else if (mode == PIN_MODE_INPUT) {
		/* input setting: not pull. */
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	} else if (mode == PIN_MODE_INPUT_PULLUP) {
		/* input setting: pull up. */
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	} else if (mode == PIN_MODE_INPUT_PULLDOWN) {
		/* input setting: pull down. */
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	} else if (mode == PIN_MODE_OUTPUT_OD) {
		/* output setting: od. */
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	}

	HAL_GPIO_Init(index->gpio, &GPIO_InitStruct);
}

rt_inline rt_int32_t bit2bitno(rt_uint32_t bit)
{
	int i;
	for (i = 0; i < 32; i++) {
		if ((0x01 << i) == bit) {
			return i;
		}
	}
	return -1;
}


//rt_pin.c

static struct rt_device_pin _hw_pin;
static rt_size_t _pin_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
	struct rt_device_pin_status *status;
	struct rt_device_pin *pin = (struct rt_device_pin *)dev;

	/* check parameters */
	//RT_ASSERT(pin != RT_NULL);

	status = (struct rt_device_pin_status *) buffer;
	if (status == RT_NULL || size != sizeof(*status)) return 0;

	status->status = pin->ops->pin_read(dev, status->pin);
	return size;
}

static rt_size_t _pin_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
	struct rt_device_pin_status *status;
	struct rt_device_pin *pin = (struct rt_device_pin *)dev;

	/* check parameters */
	RT_ASSERT(pin != RT_NULL);

	status = (struct rt_device_pin_status *) buffer;
	if (status == RT_NULL || size != sizeof(*status)) return 0;

	pin->ops->pin_write(dev, (rt_base_t)status->pin, (rt_base_t)status->status);

	return size;
}

static rt_err_t  _pin_control(rt_device_t dev, int cmd, void *args)
{
	struct rt_device_pin_mode *mode;
	struct rt_device_pin *pin = (struct rt_device_pin *)dev;

	/* check parameters */
	RT_ASSERT(pin != RT_NULL);

	mode = (struct rt_device_pin_mode *) args;
	if (mode == RT_NULL) return -RT_ERROR;

	pin->ops->pin_mode(dev, (rt_base_t)mode->pin, (rt_base_t)mode->mode);

	return 0;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops pin_ops = {
	RT_NULL,
	RT_NULL,
	RT_NULL,
	_pin_read,
	_pin_write,
	_pin_control
};
#endif

/* RT-Thread Hardware PIN APIs */
void rt_pin_mode(rt_base_t pin, rt_base_t mode)
{
	RT_ASSERT(_hw_pin.ops != RT_NULL);
	_hw_pin.ops->pin_mode(&_hw_pin.parent, pin, mode);
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_pin_mode, pinMode, set hardware pin mode);

void rt_pin_write(rt_base_t pin, rt_base_t value)
{
	RT_ASSERT(_hw_pin.ops != RT_NULL);
	_hw_pin.ops->pin_write(&_hw_pin.parent, pin, value);
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_pin_write, pinWrite, write value to hardware pin);

int  rt_pin_read(rt_base_t pin)
{
	RT_ASSERT(_hw_pin.ops != RT_NULL);
	return _hw_pin.ops->pin_read(&_hw_pin.parent, pin);
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_pin_read, pinRead, read status from hardware pin);

rt_base_t rt_pin_get(const char *name)
{
	RT_ASSERT(_hw_pin.ops != RT_NULL);
	RT_ASSERT(name[0] == 'P');

	if(_hw_pin.ops->pin_get == RT_NULL) {
		return -RT_ENOSYS;
	}

	return _hw_pin.ops->pin_get(name);
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_pin_get, pinGet, get pin number from hardware pin);

#endif