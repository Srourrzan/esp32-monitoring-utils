/*
 * This file is part of benchmarking work for different ESP-CSI firmwares,
 * and this file is used to measure CPU usage for espressif ESP
 * recv router csi firmware
 */

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "monitoring_utils.h"

int init_task(t_task *t_r, int flag)
{
  t_r->n = uxTaskGetNumberOfTasks();
  t_r->arr = malloc(t_r->n * sizeof(TaskStatus_t));
  if (!t_r->arr)
	return (-1);
  if (flag)
  	t_r->c = uxTaskGetSystemState(t_r->arr, t_r->n, &(t_r->tot));
  else
	t_r->c = uxTaskGetSystemState(t_r->arr, t_r->n, NULL);
  return (0);
}

static void find_task_name(t_task *tsk_r, const char *name)
{
  for (UBaseType_t i = 0; i < tsk_r->c; ++i)
  {
	if (!strcmp((const char*)tsk_r->arr[i].pcTaskName, name))
		tsk_r->name = tsk_r->arr[i].ulRunTimeCounter;
  }
}

static float cpu_percent_for_task(const char *name, uint32_t window_ms)
{
  t_task tsk1;
  t_task tsk2;
  uint32_t dt_task;
  uint32_t dt_total;

  memset(&tsk1, 0, sizeof(tsk1));
  if(init_task(&tsk1, 1) < 0)
	return (-1.0);
  vTaskDelay(pdMS_TO_TICKS(window_ms));
  memset(&tsk2, 0, sizeof(tsk2));
  if(init_task(&tsk2, 1) < 0)
	{
	  free(tsk1.arr);
	  return (-1.0);
	}
  find_task_name(&tsk1, name);
  find_task_name(&tsk2, name);
  free(tsk1.arr);
  free(tsk2.arr);
  if (tsk1.name == 0 && tsk2.name == 0)
	return (-1.0);
  dt_task = tsk2.name - tsk1.name;
  dt_total = tsk2.tot - tsk1.tot;
  if (dt_total)
	return (100.0f * ((float)dt_task / (float)dt_total));
  return (0.0f);
}

void print_cpu_for_wifi(void)
{
  float p;

  p = cpu_percent_for_task(TASK_NAME, 1000);
  ESP_LOGI(CPU_TAG, "CSI task CPU = %.1f%%", p);
}

void print_full_task_cpu_stats(void)
{
  char *stats_buffer;

  stats_buffer = calloc(BUFFER_SIZE, sizeof(char));
  if (!stats_buffer)
	return ;
  vTaskGetRunTimeStats(stats_buffer);
  ESP_LOGI("CPU_DEBUG", "\n ALL RUNNING TASKS\n%s", stats_buffer);
  free(stats_buffer);
}
