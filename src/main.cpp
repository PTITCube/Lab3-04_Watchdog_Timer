/**
 * LAB 3 - VÍ DỤ 04: GIÁM SÁT CHÓ CANH (WATCHDOG TIMER) & TỰ PHỤC HỒI HỆ THỐNG (FDIR)
 */
#include <Arduino.h>
#include <PTITCube.h>
#include <esp_task_wdt.h>

PTIT_CDH cdh;

#define LED_PIN         2
#define BUTTON_PIN      4
#define WDT_TIMEOUT_SEC 3

static volatile bool simulateFault = false;

// Tác vụ nhịp tim & nạp lại Watchdog Timer định kỳ
void heartbeatTask(void* pvParameters) {
    esp_task_wdt_add(NULL);
    bool ledState = false;

    for (;;) {
        if (!simulateFault) {
            esp_task_wdt_reset();
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        } else {
            cdh.println("[WDT] Kích hoạt sự cố treo! Ngừng feed Watchdog -> Rebooting...");
            digitalWrite(LED_PIN, HIGH);
            while (1) {
                delay(100);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Tác vụ giám sát nút nhấn & lệnh Serial để mô phỏng lỗi
void faultMonitorTask(void* pvParameters) {
    for (;;) {
        if (digitalRead(BUTTON_PIN) == LOW) {
            simulateFault = true;
        }

        if (cdh.available() > 0) {
            char cmd = cdh.read();
            if (cmd == 'f' || cmd == 'F' || cmd == 'h' || cmd == 'H') {
                simulateFault = true;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void printResetReason(esp_reset_reason_t reason) {
    cdh.print("[BOOT] Reset Reason: ");
    switch (reason) {
        case ESP_RST_POWERON:  cdh.println("Power-on Reset"); break;
        case ESP_RST_SW:       cdh.println("Software Reset"); break;
        case ESP_RST_PANIC:    cdh.println("Panic / Exception Reset"); break;
        case ESP_RST_TASK_WDT: cdh.println("Task Watchdog Reset (TWDT - FDIR Auto-recovered)"); break;
        case ESP_RST_INT_WDT:  cdh.println("Interrupt Watchdog Reset"); break;
        default:               cdh.printf("Code: %d\n", reason); break;
    }
}

void setup() {
    cdh.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    printResetReason(esp_reset_reason());

    // Khởi tạo Task Watchdog Timer
    esp_task_wdt_init(WDT_TIMEOUT_SEC, true);

    cdh.println("[WDT] Active (Timeout: 3s). Nhấn nút GPIO 4 hoặc gửi 'f' để thử nghiệm treo.");

    xTaskCreatePinnedToCore(heartbeatTask,    "HeartbeatWDT", 2048, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(faultMonitorTask, "FaultMonitor", 2048, NULL, 1, NULL, 1);
}

void loop() {
}
