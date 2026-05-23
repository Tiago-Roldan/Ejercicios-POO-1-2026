#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "appconfig.h"

#include <QString>

class ConfigManager
{
public:
    static AppConfig load();
    static QString projectRootPath();
};

#endif
