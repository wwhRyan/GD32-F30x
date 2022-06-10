/**
 * @file at.c
 * @author Wu Wenhao (whwu@appotronics.com)
 * @brief
 * @version 1.02
 * @date 2022-02-15
 *
 * @copyright Copyright@appotronics 2022. All Rights Reserved
 *
 */
#include "main.h"
#include "AtProtocol.h"

extern asAtProtocol at_obj;

// testcase AT+Gamma=On
// IAtOperationRegister(kCmdGamma, pAt_Kv_List, pAt_feedback_str)
// {
//     asAtKvUnit_Float my_kvs[5];
//     ICastAtKvListTo(kAtValueFloat, pAt_Kv_List, my_kvs);
//     debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
//     for (int i = 0; i < pAt_Kv_List->size; i++)
//     {
//         debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
//         debug_printf("at_type : key_id = %d, value = %f\n", my_kvs[i].key, my_kvs[i].value);
//     }
//     IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
// }

// testcase AT+System=On
IAtOperationRegister(kCmdSystem, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdLightSource, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdVersion, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdLightSourceTime, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdInstallationMode, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdVerticalKeystone, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdHorizonKeystone, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdKeystone2D, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdTestPattern, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdTemperature, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdCwSpeed, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdReset, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdEeprom, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdError, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdLogInfo, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdSetCurrent, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdGetCurrent, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}

IAtOperationRegister(kCmdSn, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Enum my_kvs[1];
    ICastAtKvListTo(kAtValueEnum, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %d\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");

    if (kAtControlType == IGetAtCmdType(&at_obj))
    {
        if (kKeyOn == my_kvs[0].value)
        {
            debug_printf("system on\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else if (kKeyOff == my_kvs[0].value)
        {
            debug_printf("system off\n");
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
        else
        {
            debug_printf("system value error\n");
        }
    }
    else
    {
        if (kKeyStatus == my_kvs[0].key)
        {
            IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
        }
    }
}
