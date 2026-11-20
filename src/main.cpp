/**
 * VÍ DỤ 3-04: GIÁM SÁT HỆ THỐNG VỚI WATCHDOG TIMER & TỰ PHỤC HỒI
 */
#include <Arduino.h>
#include <PTITCube.h>
#include <esp_task_wdt.h>

PTIT_CDH cdh;

#define LED_PIN         2
#define BUTTON_PIN      4
#define WDT_TIMEOUT_SEC 3

static volatile bool simulateFault = false;

void heartbeatTask(void* pvParameters) {
    esp_task_wdt_add(NULL);
    bool ledState = false;

    for (;;) {
        if (!simulateFault) {
            esp_task_wdt_reset();
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
        } else {
            cdh.println("WDT: Treo he thong! Ngung feed WDT -> Tu dong khoi dong lai sau 3s...");
            digitalWrite(LED_PIN, HIGH);
            while (1) delay(100);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void faultMonitorTask(void* pvParameters) {
    for (;;) {
        if (digitalRead(BUTTON_PIN) == LOW) {
            simulateFault = true;
        }
        if (cdh.available() > 0) {
            String cmd = cdh.readStringUntil('\n');
            cmd.trim();
            if (cmd.equalsIgnoreCase("fault") || cmd.equalsIgnoreCase("freeze") || cmd.equalsIgnoreCase("f")) {
                simulateFault = true;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup() {
    cdh.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    esp_reset_reason_t reason = esp_reset_reason();
    cdh.printf("BOOT: Nguyen nhan khoi dong: %s\n", 
               (reason == ESP_RST_TASK_WDT) ? "Task Watchdog Reset (TWDT da tu phuc hoi)" : "Normal Power-on/Reset");

    esp_task_wdt_init(WDT_TIMEOUT_SEC, true);

    xTaskCreatePinnedToCore(heartbeatTask,    "HeartbeatWDT", 2048, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(faultMonitorTask, "FaultMonitor", 2048, NULL, 1, NULL, 1);
}

void loop() {
}
