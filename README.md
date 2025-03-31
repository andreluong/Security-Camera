# Security Camera

A real-time, multi-threaded security camera system using a BeagleY-AI board to capture, process, and stream video. 
The system leverages OpenCV and MobileNet-SSD for people detection and WebSocket++ for low-latency video streaming to a web application.
The camera is mounted on a pan/tilt kit controlled via PWM signals for adjustable viewing angles.

### 🌟 Features
- Real-time video streaming
- Adjustable camera angles via pan/tilt kit
- User-friendly web application with board control
- TODO: night light, complete joystick functionality, LCD screen, snapshots

### ⚡️ Tech Stack

**Frontend**
- React
- Typescript
- TailwindCSS
- HeroUI
- Vite

**Backend**
- C++
- OpenCV
- CMake
- BeagleY-AI
- Linux (Debian)
- VMWare

### ⚙️ Setup

1. Clone the repository
    ```
    git clone https://github.com/andreluong/Security-Camera.git
    ```

2. Setup frontend
    ```
    cd frontend
    npm install
    npm run dev
    ```

3. Setup backend
    1. Install dependencies on HOST
       ```
       sudo apt update
       sudo apt install cmake g++ pkg-config libopencv-dev g++-aarch64-linux-gnu
       ```
    2. Clone OpenCV
        ```
        git clone https://github.com/opencv/opencv.git
        cd opencv
        mkdir build && cd build
        ```
    3. Build. This would take a while
        ```
        cmake  -DCMAKE_SYSTEM_NAME=Linux  -DCMAKE_SYSTEM_PROCESSOR=aarch64  -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++  -DCMAKE_INSTALL_PREFIX=/usr/aarch64-linux-gnu  -DCMAKE_FIND_ROOT_PATH=/usr/aarch64-linux-gnu  -DBUILD_SHARED_LIBS=ON  -DBUILD_EXAMPLES=OFF  -DBUILD_TESTS=OFF  -DBUILD_PERF_TESTS=OFF  ../
        
        make -j$(nproc)
        sudo make install
        ```
    4. Create an opencv-lib folder in TARGET. Copy OpenCV build/lib to TARGET
    5. Find the video port for USB camera `v4l2-ctl --list-devices`. Project uses `/dev/video3`
    6. Build the host
        ```
        cmake .. \
        -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
        -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
        
        make
        ```

