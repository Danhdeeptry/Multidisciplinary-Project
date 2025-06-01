# Multidisciplinary-Project
# How to Run the Web Frontend

## 1. Prerequisites
- Node.js installed (latest version recommended)
- npm installed (comes with Node.js)

## 2. Steps to Run

### Step 1: Open a terminal and navigate to the frontend folder
```powershell
cd frontend
```

### Step 2: Install dependencies
Run this command the first time or whenever package.json changes:
```powershell
npm install
```

### Step 3: Start the development server
```powershell
npm run dev
```

After starting, the terminal will show a local address (usually http://localhost:5173 or similar).

### Step 4: Open your browser and go to the address above

## 3. Note for PowerShell users on Windows
If you see an "Execution Policy" error when running npm, run this command in PowerShell before using npm:
```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```
Or use Command Prompt (cmd) to run npm install/npm run dev instead.

---
If you encounter errors, please share the error log for further support.
