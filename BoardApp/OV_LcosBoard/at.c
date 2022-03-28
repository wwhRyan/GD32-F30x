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

//testcase AT+Gamma=On
IAtOperationRegister(kCmdGamma, pAt_Kv_List, pAt_feedback_str)
{
    asAtKvUnit_Float my_kvs[5];
    ICastAtKvListTo(kAtValueFloat, pAt_Kv_List, my_kvs);
    debug_printf("\n-----------------\nkvs num : %d\n", pAt_Kv_List->size);
    for (int i = 0; i < pAt_Kv_List->size; i++)
    {
        debug_printf("at_str : key = %s, value = %s\n", pAt_Kv_List->pList[i].key.pData, pAt_Kv_List->pList[i].value.pData);
        debug_printf("at_type : key_id = %d, value = %f\n", my_kvs[i].key, my_kvs[i].value);
    }
    IAddFeedbackStrTo(pAt_feedback_str, "OK\n");
}
