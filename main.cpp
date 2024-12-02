#include <iostream>
#include <string>
#include <unordered_map>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

=
const string BASE_URL = "http://short.ly/";
const string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int SHORT_URL_LENGTH = 6;


unordered_map<string, string> urlMap;         
unordered_map<string, int> clickCountMap;    
unordered_map<string, time_t> expirationMap;  
int urlCount = 0;

string generateShortUrl() {
    string shortUrl;
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> dist(0, CHARSET.size() - 1);
    
    for (int i = 0; i < SHORT_URL_LENGTH; ++i) {
        shortUrl += CHARSET[dist(generator)];
    }
    return shortUrl;
}

string shortenUrl(const string& originalUrl, int expirationHours) {
    string shortUrl;
    do {
        shortUrl = generateShortUrl();
    } while (urlMap.find(shortUrl) != urlMap.end());  

    urlMap[shortUrl] = originalUrl;
    expirationMap[shortUrl] = time(nullptr) + expirationHours * 3600;
    clickCountMap[shortUrl] = 0;
    ++urlCount;

    return BASE_URL + shortUrl;
}


string retrieveOriginalUrl(const string& shortUrl) {
    string shortUrlKey = shortUrl.substr(BASE_URL.length());
    if (urlMap.find(shortUrlKey) != urlMap.end()) {
        if (time(nullptr) > expirationMap[shortUrlKey]) {
            urlMap.erase(shortUrlKey);
            expirationMap.erase(shortUrlKey);
            clickCountMap.erase(shortUrlKey);
            return "This URL has expired and has been removed.";
        }
        ++clickCountMap[shortUrlKey];
        return urlMap[shortUrlKey];
    }
    return "Invalid or expired short URL.";
}


string getUrlStats(const string& shortUrl) {
    string shortUrlKey = shortUrl.substr(BASE_URL.length());
    if (clickCountMap.find(shortUrlKey) != clickCountMap.end()) {
        stringstream stats;
        stats << "Short URL: " << shortUrl << "\n";
        stats << "Click count: " << clickCountMap[shortUrlKey] << "\n";
        time_t expTime = expirationMap[shortUrlKey];
        stats << "Expiration time: " << put_time(localtime(&expTime), "%Y-%m-%d %H:%M:%S");
        return stats.str();
    }
    return "No stats available for the given URL.";
}

int main() {
    cout << "Welcome to the URL Shortener!\n";
    while (true) {
        cout << "\nMenu:\n1. Shorten URL\n2. Retrieve Original URL\n3. Get Stats\n4. Exit\nChoose an option: ";
        int choice;
        cin >> choice;
        cin.ignore();  // Clear input buffer before getline()

        if (choice == 1) {
            string originalUrl;
            int expirationHours;
            cout << "Enter the original URL: ";
            getline(cin, originalUrl);
            cout << "Enter expiration time (in hours): ";
            cin >> expirationHours;
            cin.ignore();  // Clear input buffer after reading expiration time

            string shortUrl = shortenUrl(originalUrl, expirationHours);
            cout << "Shortened URL: " << shortUrl << endl;
        } else if (choice == 2) {
            string shortUrl;
            cout << "Enter the short URL: ";
            getline(cin, shortUrl);
            string originalUrl = retrieveOriginalUrl(shortUrl);
            cout << "Original URL: " << originalUrl << endl;
        } else if (choice == 3) {
            string shortUrl;
            cout << "Enter the short URL: ";
            getline(cin, shortUrl);
            cout << getUrlStats(shortUrl) << endl;
        } else if (choice == 4) {
            cout << "Exiting program. Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}
