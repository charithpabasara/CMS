# Campus Canteen Management System

> A terminal-based C application for managing canteen accounts, menus, orders,
> inventory, and customer feedback.

The application is written in C and stores its records in local CSV and data
files. Students can register, sign in, browse the menu, place orders, and leave
feedback. Administrators can review feedback and complaints, see rating
averages, inspect inventory, and view the sales report.

## Features

- **Account access:** Register as a regular user or sign in with an existing
  account.
- **Student menu:** Browse food categories, add items to a cart, view the cart,
  and confirm an order.
- **Inventory updates:** Successful orders reduce the relevant menu stock.
- **Feedback:** Submit a food rating, service rating, and comment.
- **Admin portal:** Review feedback, calculate average ratings, view complaints,
  browse inventory, and inspect sales report records.
- **Input checks:** Menu choices, ratings, and account fields are validated
  before use.

## Requirements

- GCC with C17 support (the project has been built with the MSYS2 UCRT64 GCC
  toolchain on Windows).
- A terminal that supports standard input/output. The interface uses ANSI
  colors and the screen-clear command for the current operating system.

## Build

Run the following command from the project root. With MSYS2 UCRT64 installed,
open its UCRT64 terminal in this folder:

```sh
gcc -std=c17 -Wall -Wextra -Wpedantic -Ilib src/main.c src/reg.c src/login.c src/admin_auth.c src/admin_portal.c src/clearscreen.c src/buyfoods.c src/feedback.c src/showprimary.c src/studentoption.c -o cms.exe
```

In Visual Studio Code, open the project folder and run **Terminal → Run Build
Task** (or press **Ctrl+Shift+B**). The configured task builds the same
application.

`src/main.c` is the application's entry point. `src/home.c` is a separate,
legacy program with its own `main()` and is intentionally not included in this
build.

## Run

Start the application from the project root so its default `data/...` file
paths resolve correctly:

```sh
./cms.exe
```

The opening menu provides:

1. **Register** — create a regular user account. Choose a username made of
   letters, digits, or underscores, then enter a password and a 1–13 digit NIC.
2. **Login** — sign in with your username and password. Regular accounts open
   the student menu; the reserved `admin` account opens the admin portal.
3. **Exit** — close the application.

### Student menu

- **Watch Inventory** shows the available menu categories and stock.
- **Buy Foods** lets you choose a category, add item quantities to your cart,
  review it, and confirm the order.
- **Give Feedback or Complain** records an order ID, food and service ratings
  (1–5), and a comment. You can confirm or cancel before saving.

### Admin account and portal

On first admin sign-in, the application initializes `data/admin.dat` if the
file is missing or empty. The initial username is `admin`, and the default
password is `Admin@123`; the application requires changing it at first
sign-in. Once authenticated, use the admin menu to view feedback, calculate
rating averages, view complaints, browse inventory, or read the sales report.

## Data files

| Path | Purpose |
|---|---|
| `data/admin.dat` | Admin account and first-login password-change flag |
| `data/credentials.csv` | Regular user accounts |
| `data/menu/meals.csv` | Meal items, prices, and stock |
| `data/menu/snacks.csv` | Snack items, prices, and stock |
| `data/menu/beverages.csv` | Beverage items, prices, and stock |
| `data/menu/desserts.csv` | Dessert items, prices, and stock |
| `data/feedback.csv` | Submitted customer feedback |
| `data/reportdata.csv` | Sales report entries |

Keep the `data` directory in place and run the program from the project root.
The admin and regular-user files can be overridden with the `ADMIN_FILE` and
`CREDENTIALS_FILE` environment variables, respectively.

## Important security note

This is a local learning project, not production authentication software.
Account passwords are stored as plain text in local files, and password input
is visible in the terminal. Do not use real or reused passwords, and do not
commit personal account data to a public repository.
