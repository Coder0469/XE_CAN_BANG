# XE_CAN_BANG
## Đồ án môn Thiết kế hệ thống nhúng: Xe cân bằng

Thành viên: Đình Hiếu, Khánh Đăng, Hữu Đạt, Quang Hiếu

Tiến độ: Đã hoàn thành 100%

Review sản phẩm:


https://github.com/user-attachments/assets/3e823fef-421b-4f22-a7d6-7f7f73d8bee9


## Linh kiện sử dụng
1. 2 Động cơ DC GA25 12V 280RPM
2. Cảm biến đo góc MPU6050
3. Driver động cơ L298N
4. Module Bluetooth UART HC05
5. Vi điều khiển: STM32F407VET6

## Cách thức hoạt động
1. Góc nghiêng của xe được thu từ cảm biến MPU6050 với chu kỳ 5ms
2. Bộ xử lý dựa trên góc nghiêng của xe để tính toán PWM và hướng quay của bánh xe dựa trên giải thuật PID
3. Điều khiển tốc độ quay và hướng quay của bánh xe sao cho xe luôn ở trạng thái cân bằng 

## Cách tune PID
1. Đặt Kp, Ki, Kd về 0
2. Điều chỉnh Kp sao cho xe có thể lắc lư xung quanh điểm cân bằng (Setpoint). Lưu ý: Kp quá cao sẽ làm xe lắc lư với biên độ rộng, Kp quá nhỏ thì xe sẽ không đủ lực để đứng dậy khi ngã.
3. Điều chỉnh Kd sao cho xe lắc lư với biên độ nhỏ nhất, xe có thể đứng dậy mượt nhất mà ịt bị vọt lố.
4. Điều chỉnh Ki sao cho xe có thể phản ứng nhanh trước những tác động nhỏ như đẩy xe hoặc tải đồ.

## Cách điều khiển xe từ xa
1. Để điều khiển xe, cần kết nối xe với điện thoại thông qua Bluetooth
2. Điện thoại sử dụng app Adruino Bluetooth Control để gửi tín hiệu điều khiển đến xe.
3. Xe nhận tín hiệu điều khiển và chạy theo yêu cầu

## Giải thuật điều khiển xe từ xa 
- Điều khiển đi thẳng/lùi: Chỉnh Setpoint +5 hoặc -5 để xe nghiêng về phía trước hoặc phía sau. PID sẽ đảm bảo xe luôn nghiêng về một phía để xe tự đổ.
- Điều khiển rẽ trái/phải: Giảm PWM của bánh trái/phải bằng 50% PWM đáng lẽ nó phải chạy.
- Xe nhận tín hiệu từ app, quyết định xe tiến vào trạng thái nào trong các trạng thái bao gồm:
  + STOP: Xe đứng im cân bằng tại chỗ
  + FORWARD: Xe nghiêng về phía trước để chạy tiến
  + BACKWARD: Xe nghiêng về đằng sau để chạy lùi
  + TURNLEFT: PWM bánh trái giảm để xe rẽ trái
  + TURNRIGHT: PWM bánh phải giảm để xe rẽ phải
  + SPINLEFT: bánh trái luôn tiến về phía trước trong khi bánh phải vẫn làm công việc cân bằng
  + SPINRIGHT: bánh phải luôn tiến về phía trước trong khi bánh trái vẫn làm công việc cân bằng
- Trường hợp xe đạt tốc độ quá cao khiến xe đổ mà không dựng lại được, cơ chế giảm tốc sẽ được kích hoạt
- Cơ chế giảm tốc: VXL đọc số xung từ encoder từ đó tính ra tốc độ xe. Khi tốc độ xe vượt quá ngưỡng sẽ điều chỉnh lại Setpoint về cân bằng.

## RTOS: Lập lịch các tác vụ để xe vừa điều khiển, vừa cân bằng
- Chương trình được chia thành 2 task thực hiện đồng thời:
  + Task 1: Thực hiện đọc cảm biến, tính PID, điều khiển động cơ cho cân bằng
  + Task 2: Thực hiện các thao tác tiến/lùi, xoay trái/phải
- Task 1 có độ ưu tiên cao nhất, thực hiện mỗi 5ms. Task này chỉ thực hiện khi ngắt timer xảy ra. 
- Task 2 có độ ưu tiên thấp hơn. Task 1 giữ khóa của task 2, nghĩa là task 2 chỉ được phép chạy khi task 1 đã chạy xong.
- Sau khi task 2 chạy xong, nó phải trả lại khóa.

<img width="590" height="327" alt="Untitled Diagram drawio" src="https://github.com/user-attachments/assets/c5ad38b1-2c09-4bea-9e00-a85fc771a367" />

Balance robot remote control
https://youtu.be/zjSgxROftQg
