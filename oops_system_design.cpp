#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm> // For std::find_if

using namespace std;

class User {
protected:
    string email;
    string password;

public:
    User(const string &email, const string &password) : email(email), password(password) {}

    void setPassword(const string &newPassword) {
        password = newPassword;
    }

    bool verifyPassword(const string &inputPassword) {
        return password == inputPassword;
    }

    void resetPassword(const string &newPassword) {
        setPassword(newPassword);
    }

    string getEmail() const {
        return email;
    }
};

class Admin;
class Manager;

class Location {
public:
    string name;
    double latitude;
    double longitude;

    Location(const string &name, double latitude, double longitude)
        : name(name), latitude(latitude), longitude(longitude) {}
};

class TravelerCompanion;  // Forward declaration

class Trip {
    int tripId;
    Location startLocation;
    Location endLocation;
    Location currentLocation;
    vector<TravelerCompanion*> companions;
    int travelerId;
    bool isOver;

public:
    Trip(int id, const Location &start, const Location &end, int travelerId)
        : tripId(id), startLocation(start), endLocation(end), currentLocation(start), travelerId(travelerId), isOver(false) {}

    void updateLocation(const Location &newLocation) {
        currentLocation = newLocation;
        if (currentLocation.name == endLocation.name) {
            isOver = true;
            notifyCompanions();
        }
    }

    void addCompanion(TravelerCompanion *companion) {
        companions.push_back(companion);
    }

    void notifyCompanions();

    bool isTripOver() const {
        return isOver;
    }

    Location getCurrentLocation() const {
        return currentLocation;
    }

    int getTripId() const {
        return tripId;
    }

    int getTravelerId() const {
        return travelerId;
    }
};

class Manager {
    vector<Trip> trips;
    map<int, vector<pair<int, string>>> feedbacks;  // tripId -> list of (travelerId, feedback)

public:
    void addTrip(const Trip &trip) {
        trips.push_back(trip);
    }

    void addFeedback(int tripId, int travelerId, const string &feedback) {
        feedbacks[tripId].emplace_back(travelerId, feedback);
    }

    vector<Trip> getAllTrips() const {
        return trips;
    }

    map<int, vector<pair<int, string>>> getAllFeedbacks() const {
        return feedbacks;
    }

    friend class Admin;
};

class Admin : public User {
public:
    Admin(const string &email, const string &password) : User(email, password) {}

    void accessAllTrips(const Manager &manager) {
        for (const auto &t : manager.getAllTrips()) {
            cout << "Trip ID: " << t.getTripId() << " Current Location: " << t.getCurrentLocation().name << endl;
        }
    }

    void accessAllFeedbacks(const Manager &manager) {
        for (const auto &f : manager.getAllFeedbacks()) {
            for (const auto &feedback : f.second) {
                cout << "Feedback for Trip ID " << f.first << " by Traveler ID " << feedback.first << ": " << feedback.second << endl;
            }
        }
    }
};

class Traveler : public User {
public:
    Traveler(const string &email, const string &password) : User(email, password) {}

    Trip initiateTrip(int id, const Location &start, const Location &end) {
        return Trip(id, start, end, /*travelerId=*/1);
    }

    void shareLocation(const Trip &trip, TravelerCompanion &companion, const string &contact, bool viaWhatsApp = false);
};

class TravelerCompanion : public User {
    Trip *currentTrip;
    string contactNumber;

public:
    TravelerCompanion(const string &email, const string &password, const string &contactNumber)
        : User(email, password), currentTrip(nullptr), contactNumber(contactNumber) {}

    void followTrip(Trip &trip) {
        currentTrip = &trip;
        cout << "Now tracking trip ID: " << trip.getTripId() << endl;
    }

    void updateLocation(const Location &newLocation) {
        if (currentTrip) {
            currentTrip->updateLocation(newLocation);
            cout << "Current location updated to: " << newLocation.name << endl;
        }
    }

    Location getCurrentLocation() const {
        return currentTrip ? currentTrip->getCurrentLocation() : Location("", 0, 0);
    }

    void submitFeedback(Manager &manager, const string &feedback) {
        if (currentTrip) {
            manager.addFeedback(currentTrip->getTripId(), currentTrip->getTravelerId(), feedback);
        }
    }

    string getContactNumber() const {
        return contactNumber;
    }
};

void Trip::notifyCompanions() {
    for (auto *companion : companions) {
        companion->updateLocation(currentLocation);
    }
}

void Traveler::shareLocation(const Trip &trip, TravelerCompanion &companion, const string &contact, bool viaWhatsApp) {
    if (companion.getContactNumber() == contact) {
        if (viaWhatsApp) {
            cout << "Sharing location via WhatsApp with " << contact << endl;
        } else {
            cout << "Sharing location via SMS with " << contact << endl;
        }
        companion.followTrip(const_cast<Trip&>(trip));  // Update the companion's current trip
        companion.updateLocation(trip.getCurrentLocation());  // Update the location for the companion
    } else {
        cout << "No traveler companion with the provided contact number!" << endl;
    }
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void loginTraveler(Traveler &traveler, Manager &manager, vector<TravelerCompanion> &companions) {
    while (true) {
        cout << "Traveler Menu: \n1. New Trip\n2. Share Ride\n3. Logout\nEnter choice: ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            int tripId = manager.getAllTrips().size() + 1;
            Location start("Start Location", 12.9716, 77.5946);
            Location end("End Location", 13.0358, 77.6333);
            Trip trip = traveler.initiateTrip(tripId, start, end);
            for (auto &companion : companions) {
                trip.addCompanion(&companion);
            }
            manager.addTrip(trip);
            cout << "New trip initiated with ID: " << tripId << endl;
        } else if (choice == 2) {
            int tripId;
            cout << "Enter Trip ID to share: ";
            cin >> tripId;
            auto trips = manager.getAllTrips();
            auto it = find_if(trips.begin(), trips.end(), [tripId](const Trip &trip) { return trip.getTripId() == tripId; });
            if (it != trips.end()) {
                Trip &trip = *it;
                string contact;
                bool viaWhatsApp;
                cout << "Enter contact: ";
                cin >> contact;
                cout << "Share via WhatsApp? (1 for Yes, 0 for No): ";
                cin >> viaWhatsApp;
                auto companionIt = find_if(companions.begin(), companions.end(), [&contact](const TravelerCompanion &companion) {
                    return companion.getContactNumber() == contact;
                });
                if (companionIt != companions.end()) {
                    traveler.shareLocation(trip, *companionIt, contact, viaWhatsApp);
                } else {
                    cout << "No traveler companion with the provided contact number!" << endl;
                }
            } else {
                cout << "Invalid Trip ID!" << endl;
            }
        } else if (choice == 3) {
            clearScreen();
            break;
        } else {
            cout << "Invalid choice!" << endl;
        }
    }
}

void loginTravelerCompanion(TravelerCompanion &companion, Manager &manager) {
    while (true) {
        cout << "Traveler Companion Menu: \n1. Check Current Ride\n2. Fetch Current Location\n3. Give Feedback\n4. Logout\nEnter choice: ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            auto currentLocation = companion.getCurrentLocation();
            if (currentLocation.name.empty()) {
                cout << "No current ride being tracked." << endl;
            } else {
                cout << "Current ride location: " << currentLocation.name << endl;
            }
        } else if (choice == 2) {
            auto currentLocation = companion.getCurrentLocation();
            if (currentLocation.name.empty()) {
                cout << "No current ride being tracked." << endl;
            } else {
                cout << "Current location: " << currentLocation.name << endl;
            }
        } else if (choice == 3) {
            string feedback;
            cout << "Enter feedback: ";
            cin.ignore();
            getline(cin, feedback);
            companion.submitFeedback(manager, feedback);
            cout << "Feedback submitted." << endl;
        } else if (choice == 4) {
            clearScreen();
            break;
        } else {
            cout << "Invalid choice!" << endl;
        }
    }
}

void loginAdmin(Admin &admin, Manager &manager) {
    while (true) {
        cout << "Admin Menu: \n1. Access All Trips\n2. Access All Feedbacks\n3. Logout\nEnter choice: ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            admin.accessAllTrips(manager);
        } else if (choice == 2) {
            admin.accessAllFeedbacks(manager);
        } else if (choice == 3) {
            clearScreen();
            break;
        } else {
            cout << "Invalid choice!" << endl;
        }
    }
}

int main() {
    Manager manager;
    Traveler traveler("traveler@example.com", "password");
    TravelerCompanion companion1("companion1@example.com", "password1", "1234567890");
    TravelerCompanion companion2("companion2@example.com", "password2", "0987654321");
    Admin admin("admin@example.com", "adminpassword");

    vector<TravelerCompanion> companions = {companion1, companion2};

    while (true) {
        cout << "========================\n";
        cout << "    RIDE-SHARE APP\n";
        cout << "========================\n";
        string email, password;
        cout << "Login\nEnter email: ";
        cin >> email;
        cout << "Enter password: ";
        cin >> password;

        if (email == traveler.getEmail() && traveler.verifyPassword(password)) {
            loginTraveler(traveler, manager, companions);
        } else if ((email == companion1.getEmail() && companion1.verifyPassword(password)) ||
                   (email == companion2.getEmail() && companion2.verifyPassword(password))) {
            if (email == companion1.getEmail()) {
                loginTravelerCompanion(companion1, manager);
            } else {
                loginTravelerCompanion(companion2, manager);
            }
        } else if (email == admin.getEmail() && admin.verifyPassword(password)) {
            loginAdmin(admin, manager);
        } else {
            cout << "Invalid login details!" << endl;
        }
        clearScreen();
    }

    return 0;
}
