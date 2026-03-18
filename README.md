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


# English version

## How It Works
1. Data Acquisition: The vehicle's tilt angle is sampled from the MPU6050 sensor at a 5ms interval.
2. Processing: Based on the current tilt angle, the processor calculates the PWM (Pulse Width Modulation) and rotation direction using a PID control algorithm.
3. Execution: The system adjusts the motor speed and direction in real-time to maintain the vehicle's self-balancing state.

## PID Tuning Guide
1. Set all gains (Kp, Ki, Kd) to zero.
2. Gradually increase Kp until the robot starts to oscillate (move back and forth) around the center position.
3. Increase Kd to dampen the oscillations caused by Kp.
4. If the robot balances but slowly drifts in one direction or fails to reach the exact center, slightly increase Ki

## Remote Control Guide
1. Connectivity: To control the vehicle, establish a Bluetooth connection between the robot and your smartphone.
2. Controller App: Use the "Arduino Bluetooth Control" app (available on mobile) to transmit control signals to the vehicle.
3. Operation: Upon receiving signals, the processor decodes the commands and maneuvers the vehicle (Forward, Backward, Left, Right) accordingly.

## Remote Control Strategy
The vehicle utilizes a dynamic setpoint and differential motor control to achieve movement while maintaining balance.
1. Movement Logic Forward/Backward: Instead of directly powering the motors, the system adjusts the PID Setpoint by 5 degree. This creates a controlled lean, forcing the PID loop to drive the wheels forward or backward to "catch" the fall, resulting in constant motion.
2. Steering (Left/Right): To turn while moving, the system applies a 50% reduction to the PWM signal of the inner wheel (the side it is turning towards) relative to its calculated balancing PWM.

To prevent the robot from reaching a "point of no return" (where it moves so fast it can no longer tilt back to balance), a Deceleration Mechanism is implemented:
- Encoder Feedback: The MCU reads pulse signals from the encoders to calculate the real-time velocity.
- Automatic Correction: If the velocity exceeds a predefined safety threshold, the system overrides the current command and resets the Setpoint to the neutral (balanced) position to decelerate and regain stability.
