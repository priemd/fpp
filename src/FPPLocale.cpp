#include "common.h"
#include "FPPLocale.h"
#include "settings.h"

Json::Value LocaleHolder::locale;
std::mutex LocaleHolder::localeLock;


Json::Value LocaleHolder::GetLocale() {
    std::unique_lock<std::mutex> lock(localeLock);

    if (locale.size())
        return locale;

    std::string localeName = getSetting("Locale");

    if (localeName == "")
        localeName = "global";

    std::string localeFile = "/opt/fpp/etc/locale/";
    localeFile += localeName + ".json";

    locale = loadJSON(localeFile);

    return locale;
}
