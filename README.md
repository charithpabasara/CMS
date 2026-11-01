# Campus Canteen Management System

> A local, terminal-based C project for canteen accounts, menu browsing, food
> orders, inventory, customer feedback, and administrator reports.

CCMS stores application data in CSV and text files under `data/`. The project
contains customer and administrator workflows, as well as a report-generation
module. Some of these modules are not currently connected in the configured
build task; see [Build status](#build-status) before trying to compile.

## Features

- **Accounts:** Register regular users and sign in as a regular user or
  administrator.
- **Menu and inventory:** Browse meals, snacks, beverages, and desserts with
  prices and stock counts.
- **Ordering:** Add items to a cart, review the subtotal, confirm the order,
  and update menu stock.
- **Feedback:** Submit a positive order ID, food and service ratings (1–5), and
  a comment. Administrators can review feedback, calculate average ratings,
  and view feedback where either rating is 2 or lower.
- **Administrator reports:** The source includes options to view a generated
  report and generate a new report from `data/sales.csv`.

## Requirements

- GCC with C17 support. The configured VS Code task uses MSYS2 UCRT64 GCC on
  Windows.
- A terminal that supports standard input/output. ANSI colors, screen
  clearing, and Unicode box characters depend on terminal support.
- Keep the project directories and files together, especially `src/`, `lib/`,
  and `data/`.

## Build status

The current `.vscode/tasks.json` task does **not** build successfully as
configured. `src/admin_portal.c` calls `newreport()`, but the task omits
`src/reports.c`, where that function is defined. Adding that source alone is
not enough: `src/reports.c` also calls `html_to_pdf()`, implemented in
`src/headless.c`. The reporting sources include Windows-specific dependencies,
so a Linux build is not established by this project configuration.

The configured task's core source list is:

```text
src/main.c
src/reg.c
src/login.c
src/admin_auth.c
src/admin_portal.c
src/clearscreen.c
src/buyfoods.c
src/feedback.c
src/showprimary.c
src/studentoption.c
```

In Visual Studio Code, **Terminal → Run Build Task** (or **Ctrl+Shift+B**)
uses this incomplete list and currently fails at link time with an undefined
reference to `newreport`. The report-enabled executable has not been verified
with a corrected build command. `src/home.c` is a separate legacy program with
its own `main()` and must not be added to this application's source list.

## Run location and data paths

The application uses paths such as `../data/menu/...`,
`../data/feedback.csv`, and `../data/reports.csv`. These are resolved relative
to the **process working directory**, not the executable location.

To make these paths resolve to this repository's `data/` directory, run the
executable with the working directory set to `src/`. The configured task, when
fixed, writes `cms.exe` to the project root. From a shell opened in `src/`, run:

```sh
../cms.exe
```

Running the executable from the repository root may cause the `../data/...`
paths to point outside the project. Registration currently tries
`../data/credentials.csv` twice, so it also requires a working directory from
which that path reaches the project data directory.

## Using the application

The opening menu provides:

1. **Register** — create a regular user with a username, password, and NIC
   containing 1–13 digits.
2. **Login** — regular accounts open the student menu; the reserved `admin`
   username opens the administrator portal.
3. **Exit** — close the application.

### Customer / student menu

- **Watch Inventory** displays menu categories and stock.
- **Buy Foods** lets you select a category, add item quantities to a cart,
  review it, and confirm the order.
- **Give Feedback or Complain** records an order ID, two 1–5 ratings, and a
  comment; the customer can submit or cancel.
- The fourth student-menu option is marked **under development**.

On order confirmation, the application checks current item availability and
updates menu stock. It appends sales lines to `data/reportdata.csv`. If the
report append fails after stock has been saved, the order may already have
changed inventory.

### Administrator portal

The source provides options to view feedback, calculate rating averages, view
low-rated feedback as complaints, browse inventory, view a generated sales
report, create a sales report, and return to the main menu.

The first admin record is initialized with username `admin` and the default
password `Admin@123`; the first login requires setting a new password. If an
admin password has already been changed, use the saved password instead.

## Data files

| Path | Purpose |
|---|---|
| `data/admin.dat` | Administrator record and first-login password-change flag |
| `data/credentials.csv` | Regular-user credential records |
| `data/menu/meals.csv` | Meal catalogue and stock |
| `data/menu/snacks.csv` | Snack catalogue and stock |
| `data/menu/beverages.csv` | Beverage catalogue and stock |
| `data/menu/desserts.csv` | Dessert catalogue and stock |
| `data/feedback.csv` | Submitted customer feedback |
| `data/reportdata.csv` | Sales lines appended by the order workflow |
| `data/sales.csv` | Input consumed by the separate report-generation module |
| `data/reports.csv` | Generated administrator report data |
| `data/reports/report.html` | Generated HTML report |
| `data/reports/visual_report.pdf` | Intended PDF export destination |

`reportdata.csv` and `sales.csv` are separate files. The current source does not
automatically transfer order records from `reportdata.csv` into `sales.csv`;
report generation depends on `sales.csv` being present and populated.

The `ADMIN_FILE` environment variable can override the administrator-record
path. `CREDENTIALS_FILE` can override the regular-user credential path in
authentication code.

## Known limitations and security

- This is a local learning project, not production authentication or
  point-of-sale software.
- Passwords are stored as plain text, and password input is visible in the
  terminal. Do not use real or reused passwords or publish account files.
- Feedback order IDs are user-entered and are not validated against completed
  orders.
- The registration flow does not currently reject duplicate usernames.
- There is no database, online payment, remote access, or multi-user locking.
- The configured build task currently omits required report modules; see
  [Build status](#build-status).

For detailed workflows and troubleshooting, see
[`docs/CMS_User_Manual.docx`](docs/CMS_User_Manual.docx). 
