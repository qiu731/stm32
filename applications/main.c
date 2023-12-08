/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* defined the LED0 pin: PB1 */
#define RGBLED_Green_PIN    GET_PIN(C, 1)
#define EVENT_FLAG3 (1 << 3)
static struct rt_event event;

static void led_rgb_cicle_light(void *parameter) {
  rt_pin_mode(RGBLED_Green_PIN, PIN_MODE_OUTPUT);
    while (1)
    {
        rt_pin_write(RGBLED_Green_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(RGBLED_Green_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}

static void usart1_cicle_print(void *parameter) {
  while(1) 
  {
    rt_kprintf("usart1 is printing hello .... \n");
    rt_thread_mdelay(2000);
  }
}

static rt_err_t timeout_cb(rt_device_t dev, rt_size_t size)
{
  rt_event_send(&event, EVENT_FLAG3);
}

static void test_hwtimer(void *parameter) {
  rt_device_t hw_dev;
  rt_err_t result;
  rt_uint32_t e;
  rt_event_t event; 
  event = (rt_event_t)parameter;
  struct rt_hwtimerval delayTime;
  delayTime.sec = 0;
  delayTime.usec = 25;
  hw_dev = rt_device_find("timer16");
  if (hw_dev == RT_NULL) {
    rt_kprintf("Not have hardware timer 16 ..... \n return test_hwtimer ...... \n");
    return;
  }
  result = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
  if (result != RT_EOK) {
    rt_kprintf("Can't open hardware timer 16 ..... \n return test_hwtimer ...... \n");
    return;
  }
  rt_device_set_rx_indicate(hw_dev, timeout_cb);
  while(1) 
  {
    rt_kprintf("Timer16 Start delay...... \n");
    rt_device_write(hw_dev, 0, &delayTime, sizeof(delayTime));
    if(rt_event_recv(event, EVENT_FLAG3, 
                      RT_EVENT_FLAG_AND|RT_EVENT_FLAG_CLEAR,
                      20, &e ) != RT_EOK )
    {
      rt_kprintf("Timer16 is not end in time .....\n return test_hwtimer ...... \n");
      break;
    }
    rt_kprintf("Timer16 Deley Complete  .....\n");
    rt_thread_delay(2000);
  }
  rt_device_close(hw_dev);
}


int main(void)
{
    rt_thread_t tid_led  = RT_NULL;
    rt_thread_t tid_usart1_print = RT_NULL;
    rt_thread_t tid_hwtimer = RT_NULL;
    rt_err_t result;
    result = rt_event_init(&event, "timer_end", RT_IPC_FLAG_PRIO);
    if (result != RT_EOK) {
      rt_kprintf("init event failed.\n");
      return -1;
    }
    tid_led = rt_thread_create("led_rgb",
                              led_rgb_cicle_light, RT_NULL,
                              512, 
                              5, 5);
    tid_usart1_print = rt_thread_create("u_print",
                                        usart1_cicle_print, RT_NULL,
                                        512, 5,
                                        6
                                        );
    tid_hwtimer = rt_thread_create("hwtimer",
                                    test_hwtimer, (void *)&event,
                                    512, 5,
                                    6
                                  );

    if(tid_led != RT_NULL) 
    {
      rt_thread_startup(tid_led);
    }
    if(tid_usart1_print != RT_NULL)
    {
      rt_thread_startup(tid_usart1_print);
    }
    if(tid_hwtimer != RT_NULL)
    {
      rt_thread_startup(tid_hwtimer);
    }                         
    return RT_EOK;
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
