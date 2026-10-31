#ifndef LOCALIZATION_H
#define LOCALIZATION_H
#include <map>
#include <string>

class Localization {
public:
    std::map<std::string, std::string>& _getStrings();
    std::string getFullLanguageCode();
};

typedef std::string (*getFullLanguageCode_t)(Localization *);
typedef std::map<std::string, std::string>& (*_getStrings_t)(Localization *);                         

#endif//Localization.h
