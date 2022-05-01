#include <atmel_start.h>
#include "driver_examples.h"
#include "cellular_setup.h"
#include "iot_logging_task.h"

#ifndef CELLULAR_DO_NOT_USE_CUSTOM_CONFIG
    /* Include custom config file before other headers. */
    #include "cellular_config.h"
#endif
#include "cellular_config_defaults.h"

#define TASK1_STACK_SIZE				(640 / sizeof(portSTACK_TYPE))
#define TASK1_PRIORITY					(tskIDLE_PRIORITY + 1)

#define CELLULAR_TASK_SIZE				(1300 / sizeof(portSTACK_TYPE))
#define CELLULAR_TASK_PRIORITY			(tskIDLE_PRIORITY + 1)

#define LOGGING_TASK_STACK_SIZE         (1024 / sizeof(portSTACK_TYPE))
#define LOGGING_MESSAGE_QUEUE_LENGTH    ( 1024 )
#define LOGGING_TASK_PRIORITY			(tskIDLE_PRIORITY + 0)

static TaskHandle_t	task1_handler, cellular_task_handler;

static void CellularDemoTask(void *pvParameters);

/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
     * state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/* Hook to catch task stack overflow */
void vApplicationStackOverflowHook( TaskHandle_t xTask, 
									signed char *pcTaskName ){
	
}

/* Hook to catch insufficient heap memory */
void vApplicationMallocFailedHook(void){
	
}

/* Hook fxn called via the IDLE task */
void vApplicationIdleHook(void){
	volatile size_t xFreeStackSpace = 0U;
	xFreeStackSpace = xPortGetFreeHeapSize();
}

/* Hook fxn called once when RTOS daemon task execs for the first time */
void vApplicationDaemonTaskStartupHook( void ){
	
}
/*-----------------------------------------------------------*/

static void CellularDemoTask(void *pvParameters){
	bool retCellular = true;
	// Setup cellular
	retCellular = setupCellular();
	if(!retCellular){
		LogError( ( "Cellular failed to initialize." ) );
	}
	
	//configASSERT( retCellular == true ); // Stop here if we fail to initialize cellular.
	//vTaskSuspend(NULL);
	
	while(1){
		UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
		LogInfo(("[CellularDemoTask], Stack: %lu", uxHighWaterMark*4));
		vTaskDelay(1000);
	}
	
}

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	/* Spawn the logging task */
	if(xLoggingTaskInitialize(LOGGING_TASK_STACK_SIZE, tskIDLE_PRIORITY, LOGGING_MESSAGE_QUEUE_LENGTH ) != pdPASS){
		while(1);
	}
	
	/* Spawn the cellular task */
	if (xTaskCreate(CellularDemoTask, "Cell", CELLULAR_TASK_SIZE, NULL, CELLULAR_TASK_PRIORITY, &cellular_task_handler) != pdPASS) {
		while (1);
	}
	
	/* Spawn the example task */
//	if (xTaskCreate(TARGET_IO_example_task, "Task1", TASK1_STACK_SIZE, NULL, TASK1_PRIORITY, &task1_handler) != pdPASS) {
//		while (1);
//	}
	
	vTaskStartScheduler();
	
	return 0;
}
