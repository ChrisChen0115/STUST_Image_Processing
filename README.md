# **數位影像處理概論 - 睡意偵測 Drowsiness Detection**
## 軟體規格:
* Visual Studio 2022
* c++
## 使用套件:
* Qt : https://www.qt.io/
* OpenCV : https://opencv.org/
* dlib : http://dlib.net/
## 前置作業: 安裝dlib(OpenCV同理)
1. 下載git
```
    https://git-scm.com/downloads
```
2. 使用git下載vcpkg
```
    git clone https://github.com/Microsoft/vcpkg.git
```
4. 建置vcpkg
```
    .\vcpkg\bootstrap-vcpkg.bat
```
5. 使用vcpkg安裝欲使用的函式庫, 這邊是安裝dlib, 進入vcpkg的目錄下,輸入
```
    vcpkg install dlib:x64-windows
```
6. 在Visual Studio裡使用vcpkg
```
    vcpkg integrate project
```
7. 之後會產生一段指令 : Install-Package vcpkg.D.vcpkg -Source "D:\vcpkg\scripts\buildsystems"
    1. 打開Visual Studio
    2. 上方工具列->工具->Nuget套件管理員->套件管理器主控台
    3. 輸入上方指令, 即可安裝完dlib
### 成果展示
**not sleeping**
![圖示](result1.png)

**sleeping**
![圖示](result2.png)
### 基礎功能
* 開啟圖檔
* ROI
### 色彩空間轉換
* 轉灰階
* 轉HSV
* 二值化
### 影像資訊
* 直方圖顯示
* 直方圖等化
### 鄰域處理
* 高斯平滑
* 中值平滑
### 幾何轉換
* 平移
* 旋轉
* 仿射轉換
* 透視投影轉換
