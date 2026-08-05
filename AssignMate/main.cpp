#include <QApplication>
#include "ui/MainWindow.h"

/**
 * @brief Main entry point of the application.
 * @param argc The number of command line arguments.
 *             Although no CLI-args are passed by the user
 *             to the app, the OS always passes at least 1 -
 *             the name of the executable/executable path.
 * @param argv The array of command line arguments.
 *             Handed directly over to QApplication so Qt can
 *             automatically handle OS-level paths & built-in
 *             Qt flags.
 * @return An integer exist code (0 for successful execution)
 *
 *         This function initializes the Qt GUI framework, builds the
 *         main window and enters the event loop (app.exec()) so the
 *         application stays open & responds to user input.
 *
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show(); // display the UI
    return app.exec(); // enter event loop
}
