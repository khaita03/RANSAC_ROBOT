# Hello các bạn
Đây là đồ án tốt nghiệp năm 2025 của tôi, tập trung nghiên cứu và phát triển mô hình Robot tự hành di chuyển giữa luống cây trồng. Dự án triển khai các thuật toán xử lý dữ liệu LiDAR (RANSAC) và điều hướng (Stanley) giúp robot vận hành ổn định và chính xác trong môi trường thực nghiệm với các hàng vật thể giả định. Đây là nền tảng quan trọng để phát triển các hệ thống robot nông nghiệp thông minh trong tương lai như tự động tưới tiêu hoặc bón phân.

# Tính năng nổi bật
Điều hướng : Sử dụng dữ liệu LiDAR để quét môi trường và xác định khoảng cách giữa các luống cây, vật cản.

Xử lý thời gian thực: Thuật toán điều khiển được tối ưu hóa để robot có thể xử lý vật cản và điều chỉnh hướng đi ngay lập tức.

Cấu trúc phần cứng mạnh mẽ: Sự kết hợp giữa vi điều khiển hiệu suất cao và máy tính nhúng chuyên dụng cho AI/Robot.

# Thành phần hệ thống

## Mạch Driver
<p align="left">
<img src="https://github.com/khaita03/RANSAC_ROBOT/blob/main/pcb.png" width="1000" alt="PCB">
</p>
Sử dụng phần mềm Altium Designer để thiết kế gồm các thành phần chính hoạt động chính.  

<p align="left">
<img src="https://github.com/khaita03/RANSAC_ROBOT/blob/main/mcu.jpg" width="300" alt="MCU">      
</p>
Khối xử lý trung tâm (STM32F405RGT6).

<p align="left">
<img src="https://github.com/khaita03/RANSAC_ROBOT/blob/main/power.jpg" width="300" alt="Power_5V">      
</p>
Khối nguồn 5V cấp cho Jetson Nano (XL4015).

<p align="left">
<img src="https://github.com/khaita03/RANSAC_ROBOT/blob/main/detect.jpg" width="300" alt="Power_detect">     
</p>
Khối bảo vệ quá dòng, quá áp cho mạch nguồn 5V (sử dụng OPAMP và ACS712).   

#### **Note
Schematic của mạch sẽ được để trong folder PCB/Driver (Nếu không có phần mềm, có thể mở các file **mainboard_1 schematic.png, mainboard_2 schematic.png, power_detect schematic.png**). 

## Phần cứng (Hardware)
Bộ xử lý trung tâm: Jetson Nano B01 (Thu thập dữ liệu LiDAR và xử lý thuật toán điều hướng).

Bộ điều khiển động cơ: STM32F405RGT6 (Nhận lệnh điều khiển tốc độ, góc đánh lái chuyển đổi sang xung PWM để điều khiển động cơ DC, Servo).

Cảm biến chính: LiDAR A1M8 (Nhận diện môi trường xung quanh).

Cảm biến hỗ trợ: MPU6050 (Cân bằng và xác định hướng hướng nghiêng của robot).

Truyền động: Động cơ DC Servo kèm Encoder độ phân giải cao.

## Phần mềm (Software)
Hệ điều hành: Ubuntu, ROS (Robot Operating System).

Ngôn ngữ lập trình: C/C++, Python.

Thư viện sử dụng: rosserial, MPU6050 Library.

# Clip chạy thực tế 

https://github.com/user-attachments/assets/183887ca-86e1-4590-8065-c5278fdd8718


