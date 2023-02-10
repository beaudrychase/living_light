#ifndef SECRETS
#define SECRETS
#pragma once
#define BOTtoken "<Telegram Bot Token>"
#define CHAT_ID "<Telegram Chat Id>"
#define WIFI_SSID "<ssid>"
#define WIFI_PASS "<wifi password>"
#define OTA_PASS "<OTA update password>"

#define TIMEZONE_API_KEY "<timezonedb API Key>"
#define LATITUDE "<geographic lat>"
#define LONGITUDE "<geographic long>"
const char TIMEZONE_CERT[] = R"=EOF=(
<Copy Root Certificate>
)=EOF=";

const char SOLARTIME_CERT[] = R"=EOF=(
<Copy Root Certificate>
)=EOF=";

#endif
