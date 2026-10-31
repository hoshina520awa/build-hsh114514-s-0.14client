#pragma once
#include <map>
#include <vector>
#include "Localization.h"
class I18n  {
private:
    static I18n *instance;
public:
    static void loadLanguages(I18n *);
    static I18n *getInstance();
    
    static std::string get(std::string const&, std::vector<std::string> const&);
    Localization* getCurrentLanguage();

    static std::vector<Localization*> languages;
	static Localization* mCurrentLanguage;
};



