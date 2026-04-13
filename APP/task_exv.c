#include "task_exv.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_exv.h"

/* ===================================================================
 *  电子膨胀阀 (三花 DPF(R04)1.5D-07 DC12V) 加冷媒全开任务
 *
 *  用途: 加注冷媒时, 上电即全开膨胀阀
 *
 *  流程:
 *    1. 初始化 GPIO
 *    2. 无记忆冷启动复位: 关阀 560 步 (全行程*112%, 防失步)
 *    3. 全开阀: 开到 500 步 (100% 开度)
 *    4. 断电保持, 任务挂起
 * =================================================================== */

void Task_EXV_Process(void const *argument)
{
    (void)argument;

    /* 1. 初始化 EXV GPIO */
    BSP_EXV_Init();

    /* 2. 无记忆冷启动复位: 560步关阀调零 (防失步, 确保机械归位) */
    BSP_EXV_ResetToZero();
    vTaskDelay(pdMS_TO_TICKS(1000));

    /* 3. 全开阀: 500步开到最大 */
    BSP_EXV_SetPosition(EXV_TOTAL_STEPS, EXV_STEP_DELAY_MS);

    /* 4. 结束励磁保持后断电, 阀芯自保持机构锁定 */
    vTaskDelay(pdMS_TO_TICKS(EXV_END_EXCITE_MS));
    BSP_EXV_DeEnergize();

    /* 阀门已全开, 任务完成, 挂起等待 */
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
