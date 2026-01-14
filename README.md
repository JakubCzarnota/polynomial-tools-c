
  

# 🖩🎉 PolynomialApp

  

**PolynomialApp** is a **terminal-based C application** for exploring, analyzing, and plotting polynomials. Think of it as a mini math lab on your terminal! 🧪💻

  

It can:

  

- Parse polynomial **formulas**

  

- Analyze their **mathematical properties**

  

- Perform **polynomial arithmetic**

  

- Generate **bitmap plots**

  

- Provide a **fun, interactive terminal UI** via `ncurses` (or `PDCurses`)

  

---

  

## 🚀 Features

  
  

- Polynomial Parsing – Type formulas like `2x^3 - 5x + 1` and watch the app interpret them instantly.

- Root Analysis – Find all real roots, including multiplicities.

- Positive/Negative Value Intervals – Know where your polynomial is above or below the x-axis.

- Increasing/Decreasing Intervals – Track where the polynomial is rising or falling.

- Extreme Points – Detect local maxima and minima.

- Inflection Points – Identify where the curve changes concavity.

- Concavity & Convexity – See which parts of the graph bend upward or downward.

- Polynomial Arithmetic – Add, subtract, multiply, or divide polynomials.

- Interactive Plotting – Generate `.bmp` plots of your polynomials for visual analysis.

- Friendly UI – Navigate menus easily with arrow keys, shortcuts, and clear prompts.

---

  

## ⚙️ Prerequisites

  

-  **C compiler** (C11 compatible)

- [CMake](https://cmake.org/) ≥ 3.10

- [ncurses](https://invisible-island.net/ncurses/) for LInux/MacOS or [PDCurses](https://pdcurses.org/) for windows

- [Cmocka](https://cmocka.org/) for testing (optional)

  

---

  

## 🛠️ Running the App

  

### Linux/MacOS

```bash

git  clone  https://github.com/JakubCzarnota/polynomial-tools-c.git

cd  PolynomialApp

./run.sh

```

### Windows (experimental)

```bash
git  clone  https://github.com/JakubCzarnota/polynomial-tools-c.git
cd  PolynomialApp
```
To compile and run the project successfully, **PDCurses** must be built and placed in the expected directory structure.
#### Directory Structure
```text

project/
├─ external/
│ └─ pdcurses/
│ ├─ include/
│ │ └─ (PDCurses header files)
│ └─ lib/
│ └─ pdcurses.lib
└─ ...

```

Then the project can be compiled.
