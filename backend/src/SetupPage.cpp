#include "SetupPage.h"

String buildSetupPage(const String &setupIp)
{
    String page = "<!doctype html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    page += "<title>RobotCar WiFi Setup</title>";
    page += "<style>body{font-family:Arial,sans-serif;margin:24px;max-width:520px}input,button{box-sizing:border-box;font-size:16px;margin:8px 0;padding:12px;width:100%}button{background:#111;color:white;border:0;border-radius:6px}</style>";
    page += "</head><body><h1>RobotCar WiFi Setup</h1>";
    page += "<p>Connect the robot to the WiFi network your phone will use for control.</p>";
    page += "<form method='POST' action='/wifi'>";
    page += "<label>WiFi name</label><input name='ssid' required maxlength='32' autocomplete='off'>";
    page += "<label>Password</label><input name='password' type='password' maxlength='64'>";
    page += "<button type='submit'>Connect Robot</button></form>";
    page += "<p>Setup hotspot IP: " + setupIp + "</p>";
    page += "</body></html>";
    return page;
}
