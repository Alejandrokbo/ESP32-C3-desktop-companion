#include <Arduino.h>
#include "config.h"
#include "core/Pins.h"
#include "core/DisplayManager.h"
#include "core/TouchButton.h"
#include "core/PageManager.h"
#include "core/NetManager.h"
#include "core/PcLink.h"
#include "pages/ClockPage.h"
#include "pages/WeatherPage.h"
#include "pages/ClaudeUsagePage.h"
#include "pages/ClawMascotPage.h"
#include "pages/MatrixRefresherPage.h"

DisplayManager display;
TouchButton touch;
PageManager pages;
PcLink pcLink;

ClockPage clockPage;
WeatherPage weatherPage;
ClaudeUsagePage claudeUsagePage(pcLink);
ClawMascotPage clawMascotPage(pcLink);
MatrixRefresherPage matrixPage;

void setup() {
    pcLink.begin(115200); // Serial (USB CDC) is used both for the PC link and for logs

    display.begin();
    touch.begin();
    NetManager::begin();

    pages.addPage(&clockPage);
    pages.addPage(&weatherPage);
    pages.addPage(&claudeUsagePage);
    pages.addPage(&clawMascotPage);
    pages.addPage(&matrixPage);
    pages.begin(display.tft());
}

void loop() {
    NetManager::loop();
    pcLink.update();
    touch.update();

    if (touch.wasTapped()) {
        pages.next(display.tft());
    } else if (touch.wasLongPressed()) {
        pages.longPress(display.tft());
    }

    pages.update(display.tft());
}
