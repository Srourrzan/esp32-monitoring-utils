/*
 * This file is part of benchmarking work for different ESP-CSI firmwares,
 * and this file is used to measure RAM at runtime (heap + stack) for espressif ESP
 * recv router csi firmware
 */


#include <string.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "monitoring_utils.h"

TaskHandle_t g_csi_task_handle = NULL;

/*
 * print_heap calls the heap monitoring functions,
 * logging the current free memory,
 * minimum free memory since boot, and the largest free block.
 * These values provide a snapshot of the heap's health and fragmentation.
 */
static void print_heap(void)
{
  size_t	free_now;
  size_t	min_free;
  size_t	largest_bl;

  free_now = heap_caps_get_free_size(MALLOC_CAP_8BIT);
  min_free = heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT);
  largest_bl = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
  ESP_LOGI(RAM_TAG,
		   "Heap DRAM: free=%u B, min_free_since_boot=%u B, largest_block=%u B",
		   (unsigned)free_now, (unsigned)min_free, (unsigned)largest_bl);
}

static void print_csi_stack_if_known(void)
{
  size_t 		hw_bytes;
  
  if (g_csi_task_handle)
	{
	  hw_bytes = (size_t)uxTaskGetStackHighWaterMark(g_csi_task_handle);
	  ESP_LOGI(RAM_TAG,
			   "Wi-Fi task stack headroom: %u B",
			   (unsigned)hw_bytes);
	}
}

static void try_bind_wifi_task_once(void)
{
  UBaseType_t 	i;
  t_task 		tsk;
  const char 	*name = NULL;
  
  if (g_csi_task_handle)
	return ;
  i = 0;
  memset(&tsk, 0, sizeof(tsk));
  if (init_task(&tsk, 0) < 0)
	return ;
  while (i < tsk.c)
	{
	  name = (const char *)tsk.arr[i].pcTaskName;
	  if (name && strstr(name, "wifi"))
		{
		  g_csi_task_handle = tsk.arr[i].xHandle;
		  ESP_LOGI(RAM_TAG, "Monitoring task '%s' for CSI load/stack", name);
		  break;
		}
	  i++;
	}
  free(tsk.arr);
}

static void resource_monitor_task(void *arg)
{
  int tick;

  tick = 0;
  while(1)
	{
	  if (!g_csi_task_handle)
		try_bind_wifi_task_once();
	  print_heap(); // heap now + minimum since boot + largest block
	  print_csi_stack_if_known(); // stack headroom for the bound task (bytes)
	  if ((++tick % 5) == 0) // every ~5 seconds
		print_cpu_for_wifi();
	  vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void start_resource_monitor(void)
{
  xTaskCreate(resource_monitor_task, RAM_TAG, 4096, NULL,
			  tskIDLE_PRIORITY+1, NULL);
}
