#include <stdint.h>
#include <stdio.h>

#include "../../../../clib/proxmark3/include/usb_cmd.h"
#include "pm3dev.h"

void ShowGraphWindow(void) { printf("pm3jni: ShowGraphWindow called (stub)"); }
void HideGraphWindow(void) { printf("pm3jni: HideGraphWindow called (stub)"); }
void RepaintGraphWindow(void) { printf("pm3jni: RepaintGraphWindow called (stub)"); }

// definitions for things in ui.h
double CursorScaleFactor;
int PlotGridX, PlotGridY, PlotGridXdefault = 64, PlotGridYdefault = 64, CursorCPos = 0, CursorDPos = 0;
int PlotClock = 0, PlockClockStartIndex = 0;

int offline = 1;

// FIXME move this out of ui.c in upstream...
void iceSimple_Filter(int *data, const size_t len, uint8_t k) {
    int32_t filter_reg = 0;
    int16_t input, output;
    int8_t shift = (k <= 8) ? k : 4;

    for (size_t i = 0; i < len; ++i) {
        input = data[i];
        filter_reg = filter_reg - (filter_reg >> shift) + input;
        output = filter_reg >> shift;
        data[i] = output;
    }
}

// FIXME workaround upstream ui.h including readline.h for no reason
// FIXME and cmdhflegic.c using readline(prompt) directly?!
// We replace readline with a dialog prompt
char *readline(const char *prompt __attribute__((unused))) {
    // TODO popup dialog in android
    printf("pm3jni: readline called (stub)");
    return "";
}

void PrintAndLog(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    vprintf(fmt, args);
#pragma GCC diagnostic pop
    printf("\n");
    va_end(args);
}

void SendCommand(UsbCommand *c) {
    pm3dev_write((uint8_t *) c, sizeof(UsbCommand));
}

// unused but declared
// const char *get_my_executable_path(void) {}

// This is where all the Lua scripts and libraries are located
// Returns a user-selected directory instead of the application directory
const char *get_my_executable_directory(void) {
    // TODO let user select this directory; it's used for Lua scripting
    printf("pm3jni: get_my_executable_directory called (stub)");
    return ".";
}
