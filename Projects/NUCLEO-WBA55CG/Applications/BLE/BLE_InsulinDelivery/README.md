## __BLE_InsulinDelivery Application Description__

How to use the Insulin Delivery profile as specified by the BLE SIG.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Directory contents__

  - BLE_InsulinDelivery/Core/Inc/app_common.h 											App Common application configuration file for STM32WPAN Middleware.
  - BLE_InsulinDelivery/Core/Inc/app_conf.h 											Application configuration file for STM32WPAN Middleware.
  - BLE_InsulinDelivery/Core/Inc/app_entry.h 											Interface to the application
  - BLE_InsulinDelivery/Core/Inc/main.h 										    	Header for main.c file. This file contains the common defines of the application.
  - BLE_InsulinDelivery/Core/Inc/stm32wbaxx_hal_conf.h 									HAL configuration file.
  - BLE_InsulinDelivery/Core/Inc/stm32wbaxx_it.h 								    	This file contains the headers of the interrupt handlers.
  - BLE_InsulinDelivery/Core/Inc/stm32wbaxx_nucleo_conf.h 								STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h .
  - BLE_InsulinDelivery/Core/Inc/utilities_conf.h 										Header for configuration file for STM32 Utilities.
  - BLE_InsulinDelivery/MDK-ARM/RTE/_BLE_InsulinDelivery/RTE_Components.h	
  - BLE_InsulinDelivery/STM32_WPAN/App/app_ble.h 										Header for ble application
  - BLE_InsulinDelivery/STM32_WPAN/App/bas.h 											Header for service3.c
  - BLE_InsulinDelivery/STM32_WPAN/App/bas_app.h 										Header for service3_app.c
  - BLE_InsulinDelivery/STM32_WPAN/App/ble_conf.h 										Configuration file for BLE Middleware.
  - BLE_InsulinDelivery/STM32_WPAN/App/ble_dbg_conf.h 									Debug configuration file for BLE Middleware.
  - BLE_InsulinDelivery/STM32_WPAN/App/bms.h 											Header for service6.c
  - BLE_InsulinDelivery/STM32_WPAN/App/bms_app.h 										Header for service6_app.c
  - BLE_InsulinDelivery/STM32_WPAN/App/cts.h 											Header for service2.c
  - BLE_InsulinDelivery/STM32_WPAN/App/cts_app.h 										Header for service2_app.c
  - BLE_InsulinDelivery/STM32_WPAN/App/dis.h 											Header for service1.c
  - BLE_InsulinDelivery/STM32_WPAN/App/dis_app.h 										Header for service1_app.c
  - BLE_InsulinDelivery/STM32_WPAN/App/ias.h 											Header for service5.c
  - BLE_InsulinDelivery/STM32_WPAN/App/ias_app.h 										Header for service5_app.c
  - BLE_InsulinDelivery/STM32_WPAN/App/ids.h 											Header for service4.c
  - BLE_InsulinDelivery/STM32_WPAN/App/ids_app.h 										Header for service4_app.c
  - BLE_InsulinDelivery/STM32_WPAN/App/ids_srcp.h 										Header for cgms_srcp.c module
  - BLE_InsulinDelivery/STM32_WPAN/Target/bpka.h 										This file contains the interface of the BLE PKA module.
  - BLE_InsulinDelivery/STM32_WPAN/Target/host_stack_if.h 								This file contains the interface for the stack tasks
  - BLE_InsulinDelivery/STM32_WPAN/Target/ll_sys_if.h 									Header file for initiating system
  - BLE_InsulinDelivery/System/Config/ADC_Ctrl/adc_ctrl_conf.h 							Configuration Header for adc_ctrl.c module
  - BLE_InsulinDelivery/System/Config/CRC_Ctrl/crc_ctrl_conf.h 							Configuration Header for crc_ctrl.c module
  - BLE_InsulinDelivery/System/Config/Debug_GPIO/app_debug.h 							Real Time Debug module application APIs and signal table
  - BLE_InsulinDelivery/System/Config/Debug_GPIO/app_debug_signal_def.h 				Real Time Debug module application signal definition
  - BLE_InsulinDelivery/System/Config/Debug_GPIO/debug_config.h 						Real Time Debug module general configuration file
  - BLE_InsulinDelivery/System/Config/Flash/simple_nvm_arbiter_conf.h 					Configuration header for simple_nvm_arbiter.c module
  - BLE_InsulinDelivery/System/Config/Log/log_module.h 									Header file of the log module.
  - BLE_InsulinDelivery/System/Config/LowPower/app_sys.h 								Header for app_sys.c
  - BLE_InsulinDelivery/System/Config/LowPower/peripheral_init.h 						Header for peripheral init module
  - BLE_InsulinDelivery/System/Config/LowPower/user_low_power_config.h 					Header for user_low_power_config.c
  - BLE_InsulinDelivery/System/Interfaces/hw.h 											This file contains the interface of STM32 HW drivers.
  - BLE_InsulinDelivery/System/Interfaces/hw_if.h 										Hardware Interface
  - BLE_InsulinDelivery/System/Interfaces/stm32_lpm_if.h 								Header for stm32_lpm_if.c module (device specific LP management)
  - BLE_InsulinDelivery/System/Interfaces/timer_if.h 									configuration of the timer_if.c instances
  - BLE_InsulinDelivery/System/Interfaces/usart_if.h 									Header file for stm32_adv_trace interface file
  - BLE_InsulinDelivery/System/Modules/adc_ctrl.h 										Header for ADC client manager module
  - BLE_InsulinDelivery/System/Modules/ble_timer.h 										This header defines the timer functions used by the BLE stack
  - BLE_InsulinDelivery/System/Modules/crc_ctrl.h 										Header for CRC client manager module
  - BLE_InsulinDelivery/System/Modules/dbg_trace.h 										Header for dbg_trace.c
  - BLE_InsulinDelivery/System/Modules/otp.h 											Header file for One Time Programmable (OTP) area
  - BLE_InsulinDelivery/System/Modules/scm.h 											Header for scm.c module
  - BLE_InsulinDelivery/System/Modules/stm_list.h 										Header file for linked list library.
  - BLE_InsulinDelivery/System/Modules/temp_measurement.h 								Header for temp_measurement.c module
  - BLE_InsulinDelivery/System/Modules/utilities_common.h 								Common file to utilities
  - BLE_InsulinDelivery/System/Modules/baes/baes.h 										This file contains the interface of the basic AES software module.
  - BLE_InsulinDelivery/System/Modules/baes/baes_global.h 								This file contains the internal definitions of the AES software module.
  - BLE_InsulinDelivery/System/Modules/Flash/flash_driver.h 							Header for flash_driver.c module
  - BLE_InsulinDelivery/System/Modules/Flash/flash_manager.h 							Header for flash_manager.c module
  - BLE_InsulinDelivery/System/Modules/Flash/rf_timing_synchro.h 						Header for rf_timing_synchro.c module
  - BLE_InsulinDelivery/System/Modules/Flash/simple_nvm_arbiter.h 						Header for simple_nvm_arbiter.c module
  - BLE_InsulinDelivery/System/Modules/Flash/simple_nvm_arbiter_common.h 				Common header of simple_nvm_arbiter.c module
  - BLE_InsulinDelivery/System/Modules/MemoryManager/advanced_memory_manager.h 			Header for advance_memory_manager.c module
  - BLE_InsulinDelivery/System/Modules/MemoryManager/stm32_mm.h 						Header for stm32_mm.c module
  - BLE_InsulinDelivery/System/Modules/Nvm/nvm.h 										This file contains the interface of the NVM manager.
  - BLE_InsulinDelivery/System/Modules/RFControl/rf_antenna_switch.h 					RF related module to handle dedictated GPIOs for antenna switch
  - BLE_InsulinDelivery/System/Modules/RTDebug/debug_signals.h 							Real Time Debug module System and Link Layer signal definition
  - BLE_InsulinDelivery/System/Modules/RTDebug/local_debug_tables.h 					Real Time Debug module System and Link Layer signal
  - BLE_InsulinDelivery/System/Modules/RTDebug/RTDebug.h 								Real Time Debug module API declaration
  - BLE_InsulinDelivery/System/Modules/RTDebug/RTDebug_dtb.h 							Real Time Debug module API declaration for DTB usage
  - BLE_InsulinDelivery/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.h	Header file for the serial commands interpreter module.
  - BLE_InsulinDelivery/Core/Src/app_entry.c 											Entry point of the application
  - BLE_InsulinDelivery/Core/Src/main.c 										    	Main program body
  - BLE_InsulinDelivery/Core/Src/stm32wbaxx_hal_msp.c 									This file provides code for the MSP Initialization and de-Initialization codes.
  - BLE_InsulinDelivery/Core/Src/stm32wbaxx_it.c 										Interrupt Service Routines.
  - BLE_InsulinDelivery/Core/Src/system_stm32wbaxx.c 									CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  - BLE_InsulinDelivery/STM32CubeIDE/Application/User/Core/syscalls.c 					STM32CubeIDE Minimal System calls file
  - BLE_InsulinDelivery/STM32CubeIDE/Application/User/Core/sysmem.c 					STM32CubeIDE System Memory calls file
  - BLE_InsulinDelivery/STM32_WPAN/App/app_ble.c 										BLE Application
  - BLE_InsulinDelivery/STM32_WPAN/App/bas.c 											service3 definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/bas_app.c 										service3_app application definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/bms.c 											service6 definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/bms_app.c 										service6_app application definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/cts.c 											service2 definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/cts_app.c 										service2_app application definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/dis.c 											service1 definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/dis_app.c 										service1_app application definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/ias.c 											service5 definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/ias_app.c 										service5_app application definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/ids.c 											service4 definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/ids_app.c 										service4_app application definition.
  - BLE_InsulinDelivery/STM32_WPAN/App/ids_srcp.c 										IDS SRCP
  - BLE_InsulinDelivery/STM32_WPAN/Target/bleplat.c 									This file implements the platform functions for BLE stack library.
  - BLE_InsulinDelivery/STM32_WPAN/Target/bpka.c 										This file implements the BLE PKA module.
  - BLE_InsulinDelivery/STM32_WPAN/Target/host_stack_if.c 								Source file for the stack tasks
  - BLE_InsulinDelivery/STM32_WPAN/Target/linklayer_plat.c 								Source file for the linklayer plateform adaptation layer
  - BLE_InsulinDelivery/STM32_WPAN/Target/ll_sys_if.c 									Source file for initiating system
  - BLE_InsulinDelivery/STM32_WPAN/Target/power_table.c 								This file contains supported power tables
  - BLE_InsulinDelivery/System/Config/ADC_Ctrl/adc_ctrl_conf.c 							Source for ADC client controller module configuration file
  - BLE_InsulinDelivery/System/Config/CRC_Ctrl/crc_ctrl_conf.c 							Source for CRC client controller module configuration file
  - BLE_InsulinDelivery/System/Config/Debug_GPIO/app_debug.c 							Real Time Debug module application side APIs
  - BLE_InsulinDelivery/System/Config/Flash/simple_nvm_arbiter_conf.c 					The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs.
  - BLE_InsulinDelivery/System/Config/Log/log_module.c 									Source file of the log module.
  - BLE_InsulinDelivery/System/Config/LowPower/peripheral_init.c 						tbd module
  - BLE_InsulinDelivery/System/Config/LowPower/user_low_power_config.c 					Low power related user configuration.
  - BLE_InsulinDelivery/System/Interfaces/hw_aes.c 										This file contains the AES driver for STM32WBA
  - BLE_InsulinDelivery/System/Interfaces/hw_otp.c 										This file contains the OTP driver.
  - BLE_InsulinDelivery/System/Interfaces/hw_pka.c 										This file contains the PKA driver for STM32WBA
  - BLE_InsulinDelivery/System/Interfaces/hw_rng.c 										This file contains the RNG driver for STM32WBA
  - BLE_InsulinDelivery/System/Interfaces/pka_p256.c 									This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve.
  - BLE_InsulinDelivery/System/Interfaces/stm32_lpm_if.c 								Low layer function to enter/exit low power modes (stop, sleep)
  - BLE_InsulinDelivery/System/Interfaces/timer_if.c 									Configure RTC Alarm, Tick and Calendar manager
  - BLE_InsulinDelivery/System/Interfaces/usart_if.c 									Source file for interfacing the stm32_adv_trace to hardware
  - BLE_InsulinDelivery/System/Modules/adc_ctrl.c 										Header for ADC client manager module
  - BLE_InsulinDelivery/System/Modules/app_sys.c 										Application system for STM32WPAN Middleware.
  - BLE_InsulinDelivery/System/Modules/ble_timer.c 										This module implements the timer core functions
  - BLE_InsulinDelivery/System/Modules/crc_ctrl.c 										Source for CRC client controller module
  - BLE_InsulinDelivery/System/Modules/otp.c 											Source file for One Time Programmable (OTP) area
  - BLE_InsulinDelivery/System/Modules/scm.c 											Functions for the System Clock Manager.
  - BLE_InsulinDelivery/System/Modules/stm_list.c 										TCircular Linked List Implementation.
  - BLE_InsulinDelivery/System/Modules/temp_measurement.c 								Temp measurement module
  - BLE_InsulinDelivery/System/Modules/baes/baes_cmac.c 								This file contains the AES CMAC implementation.
  - BLE_InsulinDelivery/System/Modules/baes/baes_ecb.c 									This file contains the AES ECB functions implementation.
  - BLE_InsulinDelivery/System/Modules/Flash/flash_driver.c 							The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_InsulinDelivery/System/Modules/Flash/flash_manager.c 							The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_InsulinDelivery/System/Modules/Flash/rf_timing_synchro.c 						The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_InsulinDelivery/System/Modules/Flash/simple_nvm_arbiter.c 						The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs.
  - BLE_InsulinDelivery/System/Modules/MemoryManager/advanced_memory_manager.c 			Memory Manager
  - BLE_InsulinDelivery/System/Modules/MemoryManager/stm32_mm.c 											
  - BLE_InsulinDelivery/System/Modules/Nvm/nvm_emul.c 									This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose.
  - BLE_InsulinDelivery/System/Modules/RFControl/rf_antenna_switch.c 					RF related module to handle dedictated GPIOs for antenna switch
  - BLE_InsulinDelivery/System/Modules/RTDebug/RTDebug.c 								Real Time Debug module API definition
  - BLE_InsulinDelivery/System/Modules/RTDebug/RTDebug_dtb.c 							Real Time Debug module API definition for DTB usage
  - BLE_InsulinDelivery/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.c	Source file for the serial commands interpreter module.

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx devices.
  - Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK). 
  - This application is by default configured to support low power mode ( No traces - No debugger ) 
  - Low Power configuration can be modified in app_conf.h (CFG_FULL_LOW_POWER, CFG_DBG_SUPPORTED, CFG_DEBUG_APP_TRACE)

### __How to use it?__

In order to make the program work, you must do the following:
 - Open IAR toolchain
 - Rebuild all files and flash the board with the executable file

 On the android/ios device, enable the Bluetooth communications, and if not done before:

   - Install the ST BLE Toolbox application on the android device:
     - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
     - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
	
 Power on the Nucleo board with the BLE_InsulinDelivery application.

 You can open ST BLE Toolbox application (android/ios device).

 - Select the ID_xx in the device list and connect to the device.
 - Scroll right and select Insulin Delivery interface,
 - Pairing is supported: 
   - Button B1 clears the security database, 
   - Button B2 requests the slave req pairing, here a popup asks you to associate your device.
 - You can either bond from the smartphone by clicking on Bond Button.
 - This example supports switch to 2Mbits PHY, Button B3 is used to enable the feature.
