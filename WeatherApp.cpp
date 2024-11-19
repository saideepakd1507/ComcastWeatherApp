#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

using namespace std;

// here we are holding weather data
struct WeatherReport {
    string cityName;
    double temperature;
};

// this function to get the weather data from OpenWeather API
string getWeatherFromAPI(const string& city) {
    string key = "c004c72355408b4ca9fd32d60c96cc74";  // API key from OpenWeather API
    string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + key + "&units=metric";

    HINTERNET internetConnection, urlConnection;// Here we are creating the connection for getting response from internet
    DWORD bytesRead;
    char buffer[4096];
    string response = "";

    //Here we are initializing the internet session wth the name of WeatherApp for net communication
    internetConnection = InternetOpen(L"WeatherApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    //Here we opening the specified URL and retrieve the response
    urlConnection = InternetOpenUrlA(internetConnection, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);


    //Here reading the data from the URL and append it to the response string
    while (InternetReadFile(urlConnection, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        response.append(buffer);
    }

    //closing the sessions
    InternetCloseHandle(urlConnection);
    InternetCloseHandle(internetConnection);

    return response;//it will return the response which contain the weather data
}

// This Function is to extract weather details from raw response string
WeatherReport extractWeatherData(const string& response) {
    WeatherReport weather;

    // Finding the city name from response string
    size_t pos = response.find("\"name\":\"");
    if (pos != string::npos) {
        pos += 8;
        size_t endPos = response.find("\"", pos);
        weather.cityName = response.substr(pos, endPos - pos);
    }

    // Finding the temperature from response string
    pos = response.find("\"temp\":");
    if (pos != string::npos) {
        pos += 7;
        size_t endPos = response.find(",", pos);
        weather.temperature = stod(response.substr(pos, endPos - pos));
    }
    return weather; // returing the extracted weather data
}

// This Function is to display weather details
void displayWeatherDetails(const WeatherReport& weather) {
    if (weather.cityName.empty() || weather.temperature == -9999) {
        cout << "City not found or invalid data.\n";
    }
    else {
        cout << "City: " << weather.cityName << endl;
        cout << "Temperature: " << weather.temperature << "°C" << endl;
    }
}

// This functio is to Add city to favourites upto 3 cities
bool addCityToFavourites(vector<WeatherReport>& favourites, const WeatherReport& city) {
    if (favourites.size() < 3) { //checking if list already has 3 cities 
        favourites.push_back(city); // add city to list
        return true;
    }
    cout << "Max 3 cities allowed in favourites.\n";
    return false;
}

//This function is to Remove city from favourites list 
void removeCityFromFavourites(vector<WeatherReport>& favourites, const string& cityName) {
    for (int i = 0; i < favourites.size(); ++i) {
        if (favourites[i].cityName == cityName) {
            favourites.erase(favourites.begin() + i); // remove the city from the list
            cout << "City " << cityName << " removed from favourites.\n";
            return;
        }
    }
    cout << "City not found in favourites.\n";
}


//This function is to List all favourite cities
void listFavouriteCities(const vector<WeatherReport>& favourites) {
    if (favourites.empty()) {
        cout << "No favourite cities added yet.\n";
        return;
    }

    for (const auto& city : favourites) {
        displayWeatherDetails(city); // show the weather data from the list
    }
}

int main() {
    vector<WeatherReport> favouriteCities; //using vector to store favourite citied
    int choice; // storing the user choice 
    string cityName; //storing city name getting from user
    while (true) {
        cout << "Welcome to Comcast WeatherAPI Application\n";
        cout << "\n1. Search for Weather Details of a City\n2. Add a City to Favourites\n3. List Favourite Cities\n4. Remove from Favourites\n5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter city name: ";
            cin >> cityName;
            {
                string response = getWeatherFromAPI(cityName); // getting weather data from API
                if (!response.empty()) {
                    WeatherReport weather = extractWeatherData(response);
                    displayWeatherDetails(weather); 
                }
                else {
                    cout << "Error retrieving weather data\n";
                }
            }
            break;
        case 2:
            cout << "Enter city name to add to favourites: ";
            cin >> cityName;
            {
                string response = getWeatherFromAPI(cityName);
                if (!response.empty()) {
                    WeatherReport weather = extractWeatherData(response);
                    if (weather.cityName.empty()) {
                        cout << "City not found.\n";
                    }
                    else if (addCityToFavourites(favouriteCities, weather)) {
                        cout << "City added to favourites.\n";
                    }
                }
                else {
                    cout << "Error retrieving weather data.\n";
                }
            }
            break;
        case 3:
            cout << "List out Favourite Cities:\n";
            listFavouriteCities(favouriteCities);
            break;
        case 4:
            cout << "Enter city name to remove from favourites: ";
            cin >> cityName;
            removeCityFromFavourites(favouriteCities, cityName);
            break;
        case 5:
            cout << "Thank you for using Comcast Weather API\n";
            return 0;
        default:
            cout << "Wrong input! Please try again.\n";
        }
    }
    return 0;
}
