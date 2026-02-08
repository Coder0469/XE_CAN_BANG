# XE_CAN_BANG
## Đồ án môn Thiết kế hệ thống nhúng: Xe cân bằng

Thành viên: Đình Hiếu, Khánh Đăng, Hữu Đạt, Quang Hiếu

Tiến độ: Đã hoàn thành 100%

Review sản phẩm:

## Linh kiện sử dụng
1. 2 Động cơ DC GA25 12V 280RPM
2. Cảm biến đo góc MPU6050
3. Driver động cơ L298N
4. Module Bluetooth UART HC05
5. Vi điều khiển: STM32F407VET6

## Cách thức hoạt động
1. Góc nghiêng của xe được thu từ cảm biến MPU6050 với chu kỳ 5ms
2. Bộ xử lý dựa trên góc nghiêng của xe để tính toán tốc độ và hướng quay của bánh xe dựa trên giải thuật PID
3. Điều khiển tốc độ quay và hướng quay của bánh xe sao cho xe luôn ở trạng thái cân bằng 

## Cách tune PID
