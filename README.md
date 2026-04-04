# Hello 
Đây là đồ án tốt nghiệp của mình năm 2025. Đồ án này tập trung vào việc nghiên cứu và chế tạo một mô hình robot tự hành có khả năng di chuyển ổn định và chính xác giữa các luống cây trồng. Từ đó mở đường cho các robot tiên tiến hơn tưới cây, tưới phân. ----tối ưu hóa các công việc chăm sóc nông nghiệp như phun thuốc, bón phân hoặc giám sát cây trồng.

# Tính năng nổi bật
Điều hướng thông minh: Sử dụng dữ liệu LiDAR để quét môi trường và xác định khoảng cách giữa các luống cây, vật cản.

Xử lý thời gian thực: Thuật toán điều khiển được tối ưu hóa để robot có thể xử lý vật cản và điều chỉnh hướng đi ngay lập tức.

Cấu trúc phần cứng mạnh mẽ: Sự kết hợp giữa vi điều khiển hiệu suất cao và máy tính nhúng chuyên dụng cho AI/Robot.

# Thành phần hệ thống

Phần cứng (Hardware)

Mạch Driver
<p align="left">
| Ảnh Mạch Driver STM32 | Ảnh Robot Thực Tế |
| :---: | :---: |
| <img src="https://path-to-your-image1.png" width="300" alt="Mạch Driver"> | <img src="https://path-to-your-image2.png" width="300" alt="Robot"> |
</p>
Bộ xử lý trung tâm: Jetson Nano B01 (Thu thập dữ liệu LiDAR và xử lý thuật toán điều hướng).

Bộ điều khiển động cơ: STM32F405RGT6 (Nhận lệnh điều khiển tốc độ, góc đánh lái chuyển đổi sang xung PWM để điều khiển động cơ DC, Servo).

Cảm biến chính: LiDAR A1M8 (Nhận diện môi trường xung quanh).

Cảm biến hỗ trợ: MPU6050 (Cân bằng và xác định hướng hướng nghiêng của robot).

Truyền động: Động cơ DC Servo kèm Encoder độ phân giải cao.

Phần mềm (Software)
Hệ điều hành: Ubuntu / ROS (Robot Operating System).

Ngôn ngữ lập trình: C/C++, Python.

Thư viện sử dụng: LD06 Driver (cho LiDAR), PID Control, MPU6050 Library.
