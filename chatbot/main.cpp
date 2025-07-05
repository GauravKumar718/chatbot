#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstdlib>
#include <vector>

using namespace std;

// Utility functions
namespace Utils {
    string toLower(const string& s) {
        string result = s;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
}

// Debug settings
const bool DEBUG = false;
void debugLog(const string& msg) { if (DEBUG) cerr << "DEBUG: " << msg << endl; }

// Knowledge base storage
map<string, string> knowledge;

// Cross-platform browser launcher
class BrowserLauncher {
public:
    static bool open(const string& url) {
        cout << "🌐 Opening " << extractDomain(url) << "..." << endl;
        
        vector<string> commands = getCommands(url);
        
        for (const auto& cmd : commands) {
            debugLog("Trying: " + cmd);
            if (system(cmd.c_str()) == 0) return true;
        }
        
        cout << "❌ Couldn't open browser automatically.\n"
             << "Please visit manually: " << url << endl;
        return false;
    }

private:
    static vector<string> getCommands(const string& url) {
        return {
            #ifdef _WIN32
                "start \"\" \"" + url + "\"",
                "\"C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe\" \"" + url + "\"",
                "\"C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe\" \"" + url + "\""
            #elif __APPLE__
                "open \"" + url + "\""
            #else
                "xdg-open \"" + url + "\"",
                "google-chrome \"" + url + "\"",
                "firefox \"" + url + "\""
            #endif
        };
    }

    static string extractDomain(const string& url) {
        size_t start = url.find("//") + 2;
        size_t end = url.find('/', start);
        return url.substr(start, end - start);
    }
};

// Knowledge base manager
class KnowledgeBase {
public:
    void load() {
        ifstream db("knowledge.db");
        if (!db) {
            debugLog("No knowledge.db found - starting fresh");
            return;
        }

        string key, value;
        while (getline(db, key) && getline(db, value)) {
            knowledge[normalize(key)] = value;
            debugLog("Loaded: " + key + " => " + value);
        }
    }

    void save() {
        ofstream db("knowledge.db");
        for (const auto& entry : knowledge) {
            db << entry.first << '\n' << entry.second << '\n';
        }
        debugLog("Knowledge base saved");
    }

    string getResponse(const string& input) {
        string key = normalize(input);
        return knowledge.count(key) ? knowledge[key] : "";
    }

    void learn(const string& question, const string& answer) {
        knowledge[normalize(question)] = answer;
        save();
    }

private:
    string normalize(const string& s) {
        string result;
        for (char c : s) if (!isspace(c)) result += tolower(c);
        return result;
    }
};

// Main chatbot processor
class Chatbot {
    KnowledgeBase kb;
    map<string, string> websites = {
        {"google", "https://www.google.com"},
        {"youtube", "https://www.youtube.com"},
        {"cuims", "https://cuims.in"},
        {"chatgpt", "https://chat.openai.com"},
        {"github", "https://github.com"},
    };
ver
public:
    Chatbot() { kb.load(); }

    void process(const string& input) {
        if (input.empty()) return;

        // Check for website commands first
        for (const auto& entry : websites) {
            if (containsWord(input, entry.first)) {
                BrowserLauncher::open(entry.second);
                return;
            }
        }

        // Check knowledge base
        string response = kb.getResponse(input);
        if (!response.empty()) {
            cout << "🤖: " << response << endl;
            return;
        }

        // Learn new response
        cout << "🤖: I don't know how to respond to \"" << input << "\"\n"
             << "Please teach me (or type 'skip'): ";
        string answer;
        getline(cin, answer);
        
        if (!answer.empty() && Utils::toLower(answer) != "skip") {
            kb.learn(input, answer);
            cout << "🤖: Thank you! I'll remember that.\n";
        }
    }

private:
    bool containsWord(const string& input, const string& word) {
        string lowerInput = Utils::toLower(input);
        string lowerWord = Utils::toLower(word);
        return lowerInput.find(lowerWord) != string::npos;
    }
};

// Main program
int main() {
    #ifdef _WIN32
        system("chcp 65001 > nul"); // Enable UTF-8
        system("title Smart Chatbot");
    #endif

    cout << "╔══════════════╗\n"
         << " ║  🧠 EDITH  ║\n"
         << "  ╚══════════╝\n"
         << "Type your message or 'exit'\n\n";

    Chatbot bot;
    string input;

    while (true) {
        cout << "You: ";
        getline(cin, input);
        
        if (Utils::toLower(input) == "exit") break;
        bot.process(input);
    }

    cout << "\n🤖: Goodbye! Have a great day! 🌟\n";
    return 0;
}