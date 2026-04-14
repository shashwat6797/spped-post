You are working on a C++ Qt6 desktop application (Widgets, not QML) using CMake.

Your job is to ALWAYS follow strict project architecture and coding standards.

PROJECT STRUCTURE RULES:

* include/ → only header files (.h)
* src/ → implementation files (.cpp)
* ui/ → Qt Designer .ui files (if used)
* core/ → business logic (HTTP, parsing, etc.)
* widgets/ → reusable UI components (custom QWidget subclasses)
* models/ → data structures (Request, Response, etc.)
* services/ → logic that connects UI and core (e.g., HttpService)

NEVER mix UI code and business logic in the same class.

MainWindow should ONLY:

* handle layout
* connect signals/slots
* delegate work to services

CODING RULES:

* Use modern C++ (C++17 or later)
* Prefer smart pointers when ownership is unclear
* Use Qt parent-child memory management when possible
* Avoid raw new/delete unless required by Qt
* Use clear naming:

  * classes → PascalCase
  * variables → camelCase
  * member variables → m_prefix (e.g., m_counter)

FUNCTION RULES:

* Keep functions small and focused
* No function should exceed ~30 lines unless necessary
* Separate UI logic from business logic

QT RULES:

* Use signals/slots instead of manual callbacks
* Do not block UI thread (no long operations in UI classes)
* Use QNetworkAccessManager for HTTP
* Use layouts properly (no absolute positioning)

FILE RULES:

* One class per file
* Header and cpp must match
* No large monolithic files

GOAL:

Maintain a clean, scalable architecture suitable for a production-grade desktop application (Postman-like API client).

