# AssignMate 

### Purpose: Design a minimal, GUI-based assignment tracker 

### Framework: QT GUI 

### Features:  

- input classes 
    - input assignments from respective classes  
        - added to list 
    - include: 
        - due dates 
        - mark as complete (checkbox) 
        - assignments completed vs. to complete  
        - sort:
            - past vs. present  
- store user data locally (SQLite)  
    - use SQL as "single source of truth" 
    - data persists on program exit
    - **INSERT** new assignment
    - **UPDATE** mark as complete 
    - **SELECT** fetch & sort past vs present 
