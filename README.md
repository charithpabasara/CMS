# Campus Canteen Management System

> A local, terminal-based C project for canteen accounts, menu browsing, food
> orders, inventory, customer feedback, and administrator reports.

CCMS stores application data in CSV and text files under `data/`. The project
contains customer and administrator workflows, as well as a report-generation
module.

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
- **Administrator reports:** View a generated report or generate a new report
  from the sales data maintained by the ordering workflow.

## Requirements

- GCC with C17 support. The configured VS Code task uses MSYS2 UCRT64 GCC on
  Windows.
- A terminal that supports standard input/output. ANSI colors, screen
  clearing, and Unicode box characters depend on terminal support.
- Keep the project directories and files together, especially `src/`, `lib/`,
  and `data/`.

## Build

The repository does not include a `.vscode/tasks.json` build task. Build the
application with GCC from the repository root:

```text
gcc -std=c17 src/main.c src/reg.c src/login.c src/admin_auth.c \
  src/admin_portal.c src/clearscreen.c src/buyfoods.c src/feedback.c \
  src/showprimary.c src/studentoption.c src/reports.c src/headless.c \
  src/filter.c -o cms
```

`src/home.c` is a separate legacy program with its own `main()` and must not be
added to this application's source list.

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

1. **Register** — create a regular user with a username of at least 7
   alphanumeric characters or underscores, a password, and a NIC containing
   exactly 13 digits.
2. **Login** — regular accounts open the student menu; the reserved `admin`
   username opens the administrator portal.
3. **Exit** — close the application.

### Customer / student menu

- **Watch Inventory** displays menu categories and stock.
- **Buy Foods** lets you select a category, add item quantities to a cart,
  review it, and confirm the order.
- **Give Feedback or Complain** records an order ID, two 1–5 ratings, and a
  comment; the customer can submit or cancel.
- **Exit** returns to the main menu.

On order confirmation, the application checks current item availability,
updates menu stock, and updates `data/sales.csv` for administrator reports. It
also appends order details to `data/reportdata.csv`. If saving either sales
file fails after stock has been saved, inventory may already have changed.

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
| `data/sales.csv` | Per-item sales totals and remaining stock used by reports |
| `data/reports.csv` | Generated administrator report data |
| `data/reports/report.html` | Generated HTML report |
| `data/reports/visual_report.pdf` | Intended PDF export destination |

The order workflow updates `sales.csv` so administrator reports include
confirmed purchases. `reportdata.csv` remains a separate order-detail log.

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
- Registration rejects duplicate usernames (case-insensitively).
- There is no database, online payment, remote access, or multi-user locking.
- The configured build task currently omits required report modules; see
  [Build status](#build-status).

For detailed workflows and troubleshooting, see
[`docs/CMS_User_Manual.docx`](docs/CMS_User_Manual.docx). 
