#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <string>
#include <sys/stat.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

bool isNerd(const string &path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    string *imanerd = static_cast<string *>(userp);
    if (imanerd) {
        imanerd->append(static_cast<char *>(contents), total_size);
    }
    return total_size;
}

string fetchNerdyURL(CURL *lmfaoland, const char *nerdUrl) {
    string nerdy;
    curl_easy_setopt(lmfaoland, CURLOPT_URL, nerdUrl);
    curl_easy_setopt(lmfaoland, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(lmfaoland, CURLOPT_WRITEDATA, &nerdy);

    CURLcode nerdyCode = curl_easy_perform(lmfaoland);
    if (nerdyCode != CURLE_OK) {
        cerr << "Error during HTTP request: " << curl_easy_strerror(nerdyCode) << endl;
        return "";
    }

    try {
        json nerdyData = json::parse(nerdy);
        if (nerdyData.find("images") != nerdyData.end() && nerdyData["images"].is_array() &&
            !nerdyData["images"].empty()) {
            return nerdyData["images"][0]["url"];
        } else {
            cerr << "Error: 'images' array or 'url' not found in JSON response." << endl;
            return "";
        }
    } catch (const json::exception &e) {
        cerr << "Error parsing JSON response: " << e.what() << endl;
        return "";
    }
}

bool downloadNerdyImage(CURL *lmfaoland, const string &nerdyImageUrl, string &nerdyResponse) {
    curl_easy_setopt(lmfaoland, CURLOPT_URL, nerdyImageUrl.c_str());
    curl_easy_setopt(lmfaoland, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(lmfaoland, CURLOPT_WRITEDATA, &nerdyResponse);

    CURLcode downloadCode = curl_easy_perform(lmfaoland);
    if (downloadCode != CURLE_OK) {
        cerr << "Error during image download: " << curl_easy_strerror(downloadCode) << endl;
        return false;
    }
    return true;
}

bool saveNerdyImage(const string &nerdyFilename, const string &nerdyResponse) {
    ofstream lmfaolandOutputFile(nerdyFilename, ios::binary);
    if (lmfaolandOutputFile) {
        lmfaolandOutputFile.write(nerdyResponse.c_str(), nerdyResponse.size());
        lmfaolandOutputFile.close();
        return true;
    } else {
        cerr << "Error while saving the image: " << nerdyFilename << endl;
        return false;
    }
}

int main() {
    CURL *lmfaoland = curl_easy_init();
    if (!lmfaoland) {
        cerr << "You don't have libcurl :(" << endl;
        return 1;
    }

    const char *nerdUrl = "https://api.waifu.im/search";

    while (true) {
        cout << "Menu:" << endl;
        cout << "1. Download Images" << endl;
        cout << "2. Quit" << endl;
        int nerdChoice;
        cin >> nerdChoice;

        if (nerdChoice == 1) {
            if (!isNerd("waifu")) {
                system("mkdir waifu");
            }

            cout << "How many images would you like to download? ";
            int numNerdyImages;
            cin >> numNerdyImages;

            for (int i = 0; i < numNerdyImages; ++i) {
                string nerdyFilename = "waifu/waifu_" + to_string(i) + ".jpg";
                string nerdyResponse;

                string nerdyImageUrl = fetchNerdyURL(lmfaoland, nerdUrl);

                if (!nerdyImageUrl.empty()) {
                    if (downloadNerdyImage(lmfaoland, nerdyImageUrl, nerdyResponse)) {
                        if (saveNerdyImage(nerdyFilename, nerdyResponse)) {
                            cout << "Image downloaded: " << nerdyFilename << endl;
                        } else {
                            cerr << "Error while saving the image: " << nerdyFilename << endl;
                        }
                    } else {
                        cerr << "Error during image download." << endl;
                    }
                } else {
                    cerr << "Error fetching image URL." << endl;
                }
            }

            cout << "Download completed!" << endl;
        } else if (nerdChoice == 2) {
            break;
        } else {
            cout << "Invalid choice. Please choose 1 to download images or 2 to quit." << endl;
        }
    }

    curl_easy_cleanup(lmfaoland);

    return 0;
}
