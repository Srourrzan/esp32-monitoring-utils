## Prerequisites: FreeRTOS Configuration

This component utilizes internal FreeRTOS system metrics to track CPU load and task execution states via the `uxTaskGetSystemState` API. Because these kernel tracking hooks are disabled by default in a fresh ESP-IDF environment, you **must** manually enable them in your project's configuration menu before compiling, or the linker will fail with an `undefined reference to uxTaskGetSystemState` error.

### Menuconfig Step-by-Step Guide

1. Open your project configuration terminal interface:
   ```bash
   idf.py menuconfig

   ```

2. Navigate to the core FreeRTOS scheduler properties:
`Component config` ➔ `FreeRTOS` ➔ `Kernel`
3. Toggle on the following configuration keys (ensure they show an asterisk `[*]`):
* **Runtime Statistics Collection**
* Option: `[*] Enable FreeRTOS to collect run time stats`
* Macro: `CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS=y`


* **System State Tracking functions** (Select the option based on your active ESP-IDF version toolchain):
* **For ESP-IDF v4.4:** Scroll to the bottom of the kernel menu list and check `[*] Enable FreeRTOS trace facility functions`
* **For ESP-IDF v5.x:** Locate and check `[*] Enable task snapshot functions`

4. Save your updated environment layout map by pressing `S`, hit `Enter` to confirm the filename path target, and press `Q` to exit.

> **Important:** Because these modifications alter low-level FreeRTOS system core compilation headers, you must force a clean build after changing these settings for the linker to recognize the changes:
> ```bash
> idf.py fullclean
> idf.py build
> 
> ```
>

