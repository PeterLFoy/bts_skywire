
#include "em_cmu.h"
#if defined(CMU_PRESENT)

#include <stddef.h>
#include <limits.h>
#include "em_assert.h"
#include "em_bus.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_system.h"
#include "em_common.h"

/***************************************************************************//**
 * @brief
 *   Enable/disable a clock.
 *
 * @details
 *   In general, module clocking is disabled after a reset. If a module
 *   clock is disabled, the registers of that module are not accessible and
 *   reading from such registers may return undefined values. Writing to
 *   registers of clock disabled modules have no effect. One should normally
 *   avoid accessing module registers of a module with a disabled clock.
 *
 * @note
 *   If enabling/disabling a LF clock, synchronization into the low frequency
 *   domain is required. If the same register is modified before a previous
 *   update has completed, this function will stall until the previous
 *   synchronization has completed. Please refer to CMU_FreezeEnable() for
 *   a suggestion on how to reduce stalling time in some use cases.
 *
 * @param[in] clock
 *   The clock to enable/disable. Notice that not all defined clock
 *   points have separate enable/disable control, please refer to CMU overview
 *   in reference manual.
 *
 * @param[in] enable
 *   @li true - enable specified clock.
 *   @li false - disable specified clock.
 ******************************************************************************/
void CMU_ClockEnable(CMU_Clock_TypeDef clock, bool enable)
{
  volatile uint32_t *reg;
  uint32_t          bit;
  uint32_t          sync = 0;

  /* Identify enable register */
  switch ((clock >> CMU_EN_REG_POS) & CMU_EN_REG_MASK) {
#if defined(_CMU_CTRL_HFPERCLKEN_MASK)
    case CMU_CTRL_EN_REG:
      reg = &CMU->CTRL;
      break;
#endif

#if defined(_CMU_HFCORECLKEN0_MASK)
    case CMU_HFCORECLKEN0_EN_REG:
      reg = &CMU->HFCORECLKEN0;
#if defined(CMU_MAX_FREQ_HFLE)
      setHfLeConfig(CMU_ClockFreqGet(cmuClock_HFLE));
#endif
      break;
#endif

#if defined(_CMU_HFBUSCLKEN0_MASK)
    case CMU_HFBUSCLKEN0_EN_REG:
      reg = &CMU->HFBUSCLKEN0;
      break;
#endif

#if defined(_CMU_HFPERCLKDIV_MASK)
    case CMU_HFPERCLKDIV_EN_REG:
      reg = &CMU->HFPERCLKDIV;
      break;
#endif

    case CMU_HFPERCLKEN0_EN_REG:
      reg = &CMU->HFPERCLKEN0;
      break;

#if defined(_CMU_HFPERCLKEN1_MASK)
    case CMU_HFPERCLKEN1_EN_REG:
      reg = &CMU->HFPERCLKEN1;
      break;
#endif

    case CMU_LFACLKEN0_EN_REG:
      reg  = &CMU->LFACLKEN0;
      sync = CMU_SYNCBUSY_LFACLKEN0;
      break;

    case CMU_LFBCLKEN0_EN_REG:
      reg  = &CMU->LFBCLKEN0;
      sync = CMU_SYNCBUSY_LFBCLKEN0;
      break;

#if defined(_CMU_LFCCLKEN0_MASK)
    case CMU_LFCCLKEN0_EN_REG:
      reg = &CMU->LFCCLKEN0;
      sync = CMU_SYNCBUSY_LFCCLKEN0;
      break;
#endif

#if defined(_CMU_LFECLKEN0_MASK)
    case CMU_LFECLKEN0_EN_REG:
      reg  = &CMU->LFECLKEN0;
      sync = CMU_SYNCBUSY_LFECLKEN0;
      break;
#endif

#if defined(_CMU_SDIOCTRL_MASK)
    case CMU_SDIOREF_EN_REG:
      reg = &CMU->SDIOCTRL;
      enable = !enable;
      break;
#endif

#if defined(_CMU_QSPICTRL_MASK)
    case CMU_QSPI0REF_EN_REG:
      reg = &CMU->QSPICTRL;
      enable = !enable;
      break;
#endif
#if defined(_CMU_USBCTRL_MASK)
    case CMU_USBRCLK_EN_REG:
      reg = &CMU->USBCTRL;
      break;
#endif

    case CMU_PCNT_EN_REG:
      reg = &CMU->PCNTCTRL;
      break;

    default: /* Cannot enable/disable clock point */
      EFM_ASSERT(0);
      return;
  }

  /* Get bit position used to enable/disable */
  bit = (clock >> CMU_EN_BIT_POS) & CMU_EN_BIT_MASK;

  /* LF synchronization required? */
  if (sync) {
    syncReg(sync);
  }

  /* Set/clear bit as requested */
  BUS_RegBitWrite(reg, bit, enable);
}
