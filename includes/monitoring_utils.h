#ifndef MONITORING_UTILS_H
# define MONITORING_UTILS_H

# include "freertos/FreeRTOS.h"
# include "freertos/task.h"

# ifdef __cplusplus
extern "C" {
# endif

# define CPU_TAG "cpu"
# define RAM_TAG "resmon"
# define TASK_NAME "wifi"
# define BUFFER_SIZE 2048
  
typedef struct	s_task
{
  UBaseType_t	n;
  UBaseType_t	c;
  TaskStatus_t	*arr;
  uint32_t		tot;
  uint32_t      name;
}				t_task;
  
extern TaskHandle_t g_csi_task_handle;

void	print_cpu_for_wifi(void);
int 	init_task(t_task *t_r, int);
void	start_resource_monitor(void);

# ifdef __cplusplus
}
# endif
  
#endif