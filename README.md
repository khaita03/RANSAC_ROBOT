# Hello 
Đây là đồ án tốt nghiệp của mình năm 2025. Đồ án này tập trung vào việc nghiên cứu và chế tạo một mô hình robot tự hành có khả năng di chuyển ổn định và chính xác giữa các luống cây trồng. Từ đó mở đường cho các robot tiên tiến hơn tưới cây, tưới phân. ----tối ưu hóa các công việc chăm sóc nông nghiệp như phun thuốc, bón phân hoặc giám sát cây trồng.

# Tính năng nổi bật
Điều hướng thông minh: Sử dụng dữ liệu LiDAR để quét môi trường và xác định khoảng cách giữa các luống cây, vật cản.

Xử lý thời gian thực: Thuật toán điều khiển được tối ưu hóa để robot có thể xử lý vật cản và điều chỉnh hướng đi ngay lập tức.

Cấu trúc phần cứng mạnh mẽ: Sự kết hợp giữa vi điều khiển hiệu suất cao và máy tính nhúng chuyên dụng cho AI/Robot.

# Thành phần hệ thống

## Mạch Driver
<p align="left">
<img src="https://github.com/khaita03/RANSAC_ROBOT/blob/main/pcb.png" width="1000" alt="PCB">
</p>
Sử dụng phần mềm Altium Designer để thiết kế gồm các thành phần chính hoạt động chính.  

<p align="left">
<img src="https://github.com/khaita03/RANSAC_ROBOT/blob/main/mcu.jpg" width="300" alt="MCU"> <img src="https://github.com/khaita03/RANSAC_ROBOT/blob/main/power.jpg" width="300" alt="Power_5V"> <img src="https://github.com/khaita03/RANSAC_ROBOT/blob/main/detect.jpg" width="300" alt="Power_detect">     
</p>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Hình 1&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Hình 2&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Hình 3             

Khối xử lý trung tâm (STM32F405RGT6) - Hình 1.

Khối nguồn 5V cấp cho Jetson Nano (XL4015) - Hình 2.

Khối bảo vệ quá dòng, quá áp cho mạch nguồn 5V (sử dụng OPAMP và ACS712) - Hình 3.
#### **Note
Schematic của mạch sẽ được để trong folder PCB/Driver (Nếu không có phần mềm, có thể mở các file pdf cùng tên với file *.SchDoc để xem qua schematic của mạch). 

## Phần cứng (Hardware)
Bộ xử lý trung tâm: Jetson Nano B01 (Thu thập dữ liệu LiDAR và xử lý thuật toán điều hướng).

Bộ điều khiển động cơ: STM32F405RGT6 (Nhận lệnh điều khiển tốc độ, góc đánh lái chuyển đổi sang xung PWM để điều khiển động cơ DC, Servo).

Cảm biến chính: LiDAR A1M8 (Nhận diện môi trường xung quanh).

Cảm biến hỗ trợ: MPU6050 (Cân bằng và xác định hướng hướng nghiêng của robot).

Truyền động: Động cơ DC Servo kèm Encoder độ phân giải cao.

## Phần mềm (Software)
Hệ điều hành: Ubuntu / ROS (Robot Operating System).

Ngôn ngữ lập trình: C/C++, Python.

Thư viện sử dụng: LD06 Driver (cho LiDAR), PID Control, MPU6050 Library.
