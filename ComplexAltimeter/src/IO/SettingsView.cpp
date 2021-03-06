/*********************************************************************************
 * Open Altimeter
 *
 * Mid power rocket avionics software for altitude recording and dual deployment
 *
 * Copyright 2018, Jonathan Nobels
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **********************************************************************************/

#include "SettingsView.hpp"
#include "../FlightController.hpp"

void SettingsView::dismiss()
{
  resetOnNextLongPress = false;
  needsRefresh         = true;
}

void SettingsView::refresh()
{
  if (needsRefresh) {
    String deplAlt =
        String(String(F("Main:")) +
               String(FlightController::shared().deploymentAltitude));
    setText(F("==::: Settings :::=="), 0, false);
    setText(deplAlt, 1, false);
    String resetText =
        resetOnNextLongPress ? F("Press Again To Reset") : F("Hold To Reset");
    setText(resetText, 2, false);

    FSInfo fs_info;
    SPIFFS.info(fs_info);

    setText("", 3, false);
    setText(String("FS Size Kb: ") + String(fs_info.totalBytes / 1024), 4,
            false);
    setText(String("FS Used Kb: ") + String(fs_info.usedBytes / 1024), 5,
            false);
    update();
    needsRefresh = false;
  }
}

void SettingsView::shortPressAction()
{
  altIndex++;
  if (altIndex == 6) {
    altIndex = 0;
  }
  FlightController::shared().setDeploymentAltitude(altitudes[altIndex]);
  needsRefresh = true;
}

void SettingsView::longPressAction()
{
  if (resetOnNextLongPress) {
    FlightController::shared().resetAll();
    resetOnNextLongPress = false;
  }
  resetOnNextLongPress = true;
  needsRefresh         = true;
}