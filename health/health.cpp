/* SPDX-License-Identifier: Apache-2.0 */
#include <healthd/healthd.h>
#include <batteryservice/BatteryService.h>
#include <stdio.h>

void healthd_board_init(struct healthd_config*) {}
int healthd_board_battery_update(struct android::BatteryProperties* properties) {
    // This kernel exports millivolts, whereas BatteryMonitor assumes microvolts.
    FILE* file = fopen("/sys/class/power_supply/battery/voltage_now", "re");
    int millivolts = 0;
    if (file) {
        if (fscanf(file, "%d", &millivolts) == 1 && millivolts > 0)
            properties->batteryVoltage = millivolts;
        fclose(file);
    }
    return 0;
}
