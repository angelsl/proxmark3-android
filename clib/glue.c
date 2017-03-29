#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmdhw.h"
#include "cmdmain.h"
#include "cmdparser.h"
#include "proxmark3.h"
#include "sleep.h"
#include "uart.h"
#include "ui.h"

void ShowGraphWindow(void) {}
void HideGraphWindow(void) {}
void RepaintGraphWindow(void) {}
void MainGraphics(void) {}
void InitGraphics(int argc, char **argv) {}
void ExitGraphics(void) {}

// definitions for things in ui.h
double CursorScaleFactor;
int PlotGridX, PlotGridY, PlotGridXdefault = 64, PlotGridYdefault = 64, CursorCPos = 0, CursorDPos = 0;
int PlotClock = 0, PlockClockStartIndex = 0;

int offline;
int flushAfterWrite = 0;

// FIXME move this out of ui.c in upstream...
void iceSimple_Filter(int *data, const size_t len, uint8_t k){
// ref: http://www.edn.com/design/systems-design/4320010/A-simple-software-lowpass-filter-suits-embedded-system-applications
// parameter K
#define FILTER_SHIFT 4

	int32_t filter_reg = 0;
	int16_t input, output;
	int8_t shift = (k <=8 ) ? k : FILTER_SHIFT;

	for (int i = 0; i < len; ++i){

		input = data[i];
		// Update filter with current sample
		filter_reg = filter_reg - (filter_reg >> shift) + input;

		// Scale output for unity gain
		output = filter_reg >> shift;
		data[i] = output;
	}
}

// FIXME workaround upstream ui.h including readline.h for no reason
// and cmdhflegic.c using readline(prompt) directly?!
char *readline(const char *prompt) {
    return "";
}

void PrintAndLog(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
    int bufsz = vsnprintf(NULL, 0, fmt, args);
    char buf[bufsz+1];
	vsnprintf(buf, sizeof(buf), fmt, args);
    // TODO JNI the printed message to the UI
	va_end(args);
}

void SetLogFilename(char *fn) {}

void SendCommand(UsbCommand *c) {
    // TODO
}

// unused but declared
// const char *get_my_executable_path(void) {}

const char *get_my_executable_directory(void) {
    // TODO
    return ".";
}