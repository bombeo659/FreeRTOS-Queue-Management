#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

QueueHandle_t xQueue;

typedef enum
{
    ANGLE,
    SPEED
} DataSource_t;

typedef struct
{
    float value;
    DataSource_t dataSource;
} Data_t;

float RandomFloat(float a, float b)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

/*-----------------------------------------------------------*/

static void vAngleSenderTask(void *pvParameters)
{
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);
    Data_t txBuffer;
    srand(time(NULL));

    /* As per most tasks, this task is implemented within an infinite loop. */
    for (;;)
    {
        txBuffer.dataSource = ANGLE;
        txBuffer.value = RandomFloat(-1, 1);

        xStatus = xQueueSendToBack(xQueue, &txBuffer, xTicksToWait);

        if (xStatus != pdPASS)
        {
            printf("Could not send to the queue.\r\n");
        }
        else 
        {
            printf("Task Angle is sent.\r\n");
        }
        //vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void vSpeedSenderTask(void *pvParameters)
{
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);
    Data_t txBuffer;
    srand(time(NULL));

    /* As per most tasks, this task is implemented within an infinite loop. */
    for (;;)
    {
        txBuffer.dataSource = SPEED;
        txBuffer.value = RandomFloat(20, 30);

        xStatus = xQueueSendToBack(xQueue, &txBuffer, xTicksToWait);

        if (xStatus != pdPASS)
        {
            printf("Could not send to the queue.\r\n");
        }
        else 
        {
            printf("Task Speed is sent.\r\n");
        }
        //vTaskDelay(pdMS_TO_TICKS(500));
    }
}
/*-----------------------------------------------------------*/
static void vReceiverTask(void *pvParameters)
{
    Data_t xReceivedStructure;

    for (;;)
    {
	  	if(uxQueueMessagesWaiting(xQueue) == 3)
	 	{
			printf("Queue is full\r\n");
		}
        if (xQueueReceive(xQueue, &xReceivedStructure, 0) == pdPASS)
        {
            if (xReceivedStructure.dataSource == ANGLE)
            {
                printf("Set value of angle = %f\n", xReceivedStructure.value);
            }
            else
            {
                printf("Set value of speed = %f\n", xReceivedStructure.value);
            }
        }
        //vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    xQueue = xQueueCreate(3, sizeof(Data_t));
    

    if (xQueue != NULL)
    {
        xTaskCreatePinnedToCore(&vAngleSenderTask, "Sender", 1024 * 5, NULL, 2, NULL, 0);
        xTaskCreatePinnedToCore(&vSpeedSenderTask, "Sender", 1024 * 5, NULL, 2, NULL, 0);

        xTaskCreatePinnedToCore(&vReceiverTask, "Receiver", 1024 * 5, NULL, 1, NULL, 0);
    }
    else
    {
        printf("The queue could not be created.\r\n");
    }
}