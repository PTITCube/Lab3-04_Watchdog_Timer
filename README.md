# Lab3-04_Watchdog_Timer

**Cơ chế Giám sát Chó Canh (Watchdog Timer - WDT) & Phục hồi Lỗi FDIR**

Bài thực hành thuộc Phân hệ C&DH (Lab 3) trong hệ thống PTITCube.

## 1. Mục tiêu
- Cấu hình và kích hoạt **Task Watchdog Timer (TWDT)** trên ESP32 để giám sát các tác vụ C&DH của CubeSat.
- Nắm vững chu trình "cho chó ăn / đá chó canh" (`esp_task_wdt_reset()`) để báo hiệu tác vụ vẫn đang hoạt động bình thường.
- Thực hành kỹ thuật **Bơm lỗi (Fault Injection)** thông qua Nút nhấn (GPIO 4) hoặc lệnh Serial ('h'/'f') để mô phỏng sự cố treo hệ thống do bức xạ vũ trụ (Single Event Upset).
- Kiểm chứng khả năng tự động cưỡng bức khởi động lại (Hardware Reset) của Watchdog Timer và phân tích nguyên nhân reset (`esp_reset_reason()`).

## 2. Phần cứng sử dụng
- **ESP32 NodeMCU OBC**
- **Heartbeat LED:** GPIO 2 (tích hợp trên bo mạch, từ Lab 1).
- **Nút nhấn bơm lỗi:** GPIO 4 (`INPUT_PULLUP`, từ Lab 1).
- **Serial Monitor:** Cổng USB UART (Baudrate: `115200`).

## 3. Các bước thực nghiệm
1. Nạp code và mở Serial Monitor ở tốc độ 115200 baud.
2. Quan sát LED GPIO 2 nhấp nháy đều đặn mỗi 500ms, Serial in thông báo đá chó canh thành công.
3. Nhấn Nút nhấn (GPIO 4) hoặc gửi ký tự `h` vào Serial Monitor: Tác vụ ngừng đá chó canh và rơi vào vòng lặp vô hạn.
4. Sau 3 giây (Timeout của WDT), vi điều khiển tự động reset cứng.
5. Khi khởi động lại, hệ thống nhận diện `ESP_RST_TASK_WDT`, in thông báo phục hồi FDIR thành công và nháy LED 5 lần để báo hiệu.
