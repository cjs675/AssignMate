# AssignMate 

<p align="center">
    <img src="assets/AssignMateLogo_no_bg.png" alt="AssignMate Logo" width = "60%" /> 
</p>

### Purpose: Design a minimal, GUI-based assignment tracker 

### Framework: QT GUI 

AssignMate is a lightweight, GUI-based desktop app written in modern C++ used to track university courses and their assignments.
This project was built using core OOP principles, GUI event-driven programming, and dynamic memory management using the Qt Framework. 

### Features
- **Course Management**: add dynamically generated courses 
- **Assignment Tracking**: input individual assignments (Homework, Exams) to their given courses 
- **Aggregated Dashboard**: View all assignments across all courses 
- **Dynamic Table UI**: Assignments include titles, due dates, interactive completion checkboxes, and specific polymorphic details (ex. exam duration, homework task count)

### Technical Architecture
The application was built from scratch using pure C++ & core Qt modules, such as:<br>
- **QtWidgets** (main library that contains all visual UI elements)<br>
- **QApplication** (base manager class that runs the infinite event loop)<br>
- **QMainWindow** (base-inherited class that builds the main UI canvas and holds layouts of UI elements)<br>


#### Core OOP Concepts Used: 
- **Encapsulation & Data Hiding:** strict use of **private** member variables accessed only through **public** Getters/Setters 
- **Composition:** The ```Course``` class acts as the owner of an interal ```QVector```, managing the memory of the assignments it aggregates 
- **Inheritance & Runtime Polymorphism:** 
    - An abstract ```Assignment``` base class containing a **pure virtual function**
    - ```Exam``` and ```Homework``` derived classes that override the virtual function 
    - A ```QVector<Assignment*>``` that uses pointers to store mixed data types and dynamically invoke the correct overriden methods at runtime 

#### UI Styling 
- The application uses an external Qt Stylesheet ```style.qss``` loaded at runtime via ```QFile``` and ```QTextStream```<br>
- The sheet is written to format the window in a sleek, modern 'dark-mode' aesthetic.<br> 
- A ```try-catch``` block is used to handle exceptions if the stylesheet is missing/cannot be located.<br> 
