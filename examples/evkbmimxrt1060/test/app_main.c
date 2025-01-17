/*
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 */


/**
 * @brief Contains the application main function which spawns different demo tasks
 * included in the project.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "kvstore.h"
#include "mqtt_agent_task.h"


#define appmainRUN_QUALIFICATION_TEST_SUITE       ( 1 )

#define appmainRUN_DEVICE_ADVISOR_TEST_SUITE      ( 1 )

#define appmainRUN_OTA_END_TO_END_TEST_SUITE      ( 0 )


#define appmainMQTT_OTA_UPDATE_TASK_STACK_SIZE    ( 4096 )
#define appmainMQTT_OTA_UPDATE_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1 )

#define appmainTEST_TASK_STACK_SIZE               ( 6144 )
#define appmainTEST_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )

/**
 * @brief Stack size and priority for MQTT agent task.
 * Stack size is capped to an adequate value based on requirements from MbedTLS stack
 * for establishing a TLS connection. Task priority of MQTT agent is set to a priority
 * higher than other MQTT application tasks, so that the agent can drain the queue
 * as work is being produced.
 */
#define appmainMQTT_AGENT_TASK_STACK_SIZE         ( 6144 )
#define appmainMQTT_AGENT_TASK_PRIORITY           ( tskIDLE_PRIORITY + 2 )

extern void prvQualificationTestTask( void * pvParameters );

extern void vSubscribePublishTestTask( void * pvParameters );

int app_main( void )
{
    BaseType_t xResult = pdFAIL;

    xResult = KVStore_init();
#if ( appmainRUN_QUALIFICATION_TEST_SUITE == 1 )
    {
        if( xResult == pdPASS )
        {
            xResult = xTaskCreate( prvQualificationTestTask,
                                   "TEST",
                                   appmainTEST_TASK_STACK_SIZE,
                                   NULL,
                                   appmainTEST_TASK_PRIORITY,
                                   NULL );
        }
    }
#endif /* if ( appmainRUN_QUALIFICATION_TEST_SUITE == 1 ) */



#if ( appmainRUN_DEVICE_ADVISOR_TEST_SUITE == 1 )
    {
        if( xResult == pdPASS )
        {
            xResult = xMQTTAgentInit( appmainMQTT_AGENT_TASK_STACK_SIZE, appmainMQTT_AGENT_TASK_PRIORITY );
        }

        if( xResult == pdPASS )
        {
            xResult = xTaskCreate( vSubscribePublishTestTask,
                                   "TEST",
                                   appmainTEST_TASK_STACK_SIZE,
                                   NULL,
                                   appmainTEST_TASK_PRIORITY,
                                   NULL );
        }
    }
#endif /* if ( appmainRUN_DEVICE_ADVISOR_TEST_SUITE == 1 ) */

    return pdPASS;
}
