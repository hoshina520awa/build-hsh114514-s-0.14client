#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

class Minecraft;
class GuiMessage;
class Player;

#include <string>
#include <bits/unique_ptr.h>

// Size : 16
struct Token{
    typedef std::vector<Token> List;

    enum class Type : int {
        String,
        Number,
    };

public:
    Token(std::string const&); // _ZN5TokenC1ERKSs
    std::string const& getText(std::string const&) const; // _ZNK5Token7getTextERKSs
    int getValue(int) const; // _ZNK5Token8getValueEi
    bool getBool(bool) const; // _ZNK5Token7getBoolEb
    bool compatibleWith(Token::Type) const; // _ZNK5Token14compatibleWithENS_4TypeE
    void _parseRandom(); // _ZN5Token12_parseRandomEv
    static Token tokenize(const std::string &);
};

class TokenArray {
private:
    uintptr_t base;
    static constexpr size_t TOKEN_SIZE = 16;
public:
    TokenArray(const std::vector<Token>& vec) {
        base = reinterpret_cast<uintptr_t>(vec.data());
    }
    const Token& operator[](size_t index) const {
        return *reinterpret_cast<const Token*>(base + (index * TOKEN_SIZE));
    }
};


class ServerCommandParser{
public:
    class Command{
    public:
        Command(std::function<std::string (std::vector<Token, std::allocator<Token> > const&)> const&, std::string const&); // _ZN19ServerCommandParser7CommandC2ERKSt8functionIFSsRKSt6vectorI5TokenSaIS3_EEEERKSs
        void checkParameters(std::vector<Token, std::allocator<Token> > const&); // _ZN19ServerCommandParser7Command15checkParametersERKSt6vectorI5TokenSaIS2_EE
    };

public:
    std::string what; // 0
    std::unordered_multimap<std::string, std::unique_ptr<Command>> commands; // 4

    ServerCommandParser(Minecraft *);
    std::string result(const std::string &);
    void addCommand(std::string const&, std::string const&, std::function<std::string (std::vector<Token, std::allocator<Token> > const&)> const&); // _ZN19ServerCommandParser10addCommandERKSsS1_RKSt8functionIFSsRKSt6vectorI5TokenSaIS4_EEEE
    void executeCommand(const GuiMessage &);
};
