# 🐸 Crossing Road Game

Game Crossing Road làm bằng C++ với SFML 2.5.1.

---

## 📋 Yêu cầu hệ thống

- **Windows 10/11** (64-bit)
- **Visual Studio 2022** Community/Professional (có C++ Desktop Development workload)  
  _hoặc_  
- **VS Code** + **Build Tools for Visual Studio 2022**

---

## 🚀 Cách chạy — Visual Studio 2022

> **Chỉ cần 3 bước, không cần cài thêm gì:**

1. **Clone repo:**
   ```
   git clone <repo-url>
   ```

2. **Mở solution:**  
   Double-click vào `CrossingRoad.sln`

3. **Build & Run:**  
   Chọn cấu hình **`Debug | x64`** → nhấn **`F5`**  
   _(lần đầu build sẽ mất ~30 giây)_

> ✅ SFML đã được include sẵn trong thư mục `packages/` — không cần cài hay restore gì thêm.

---

## 🚀 Cách chạy — VS Code

> **Yêu cầu:** Phải cài [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) (chọn workload **"C++ build tools"**)

1. **Clone repo:**
   ```
   git clone <repo-url>
   ```

2. **Mở thư mục trong VS Code:**
   ```
   code CrossingRoad
   ```

3. **Cài extension C/C++ của Microsoft:**  
   Ctrl+Shift+X → tìm `C/C++` → Install

4. **Build:**  
   Nhấn **`Ctrl+Shift+B`** → chọn **"Build CrossingRoad (Debug)"**

5. **Chạy game:**  
   Nhấn **`F5`**

---

## 📁 Cấu trúc thư mục

```
CrossingRoad/
├── src/                    # Source code C++
│   ├── States/             # GameState, MenuState, ...
│   ├── Entities/           # CPEOPLE, CCAR, CTRUCK, ...
│   ├── Managers/           # SaveManager, ResourceManager
│   ├── Core/               # CGAME, State base class
│   └── UI/                 # TextBox, Button
├── assets/
│   ├── textures/           # Hình ảnh sprites
│   ├── fonts/              # Font chữ
│   └── data/               # Save files (tự tạo khi chơi)
├── packages/               # SFML 2.5.1 (đã có sẵn)
├── CrossingRoad.sln        # Visual Studio Solution
└── README.md
```

---

## ⚠️ Lưu ý

- Phải dùng cấu hình **x64** (không dùng x86)
- File `.exe` sau khi build nằm ở `x64/Debug/CrossingRoad.exe`
- Save files được lưu ở `assets/data/` — không commit folder này lên git
