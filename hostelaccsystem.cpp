#include <iostream>   
#include <fstream>    
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Room class to represent each hostel room
class Room {
private:
    int roomId;
    bool available;

public:
    // Constructor
    Room(int id) {
    roomId = id;
    available = true;
    }

    int getRoomId() const {
        return roomId;
    }

    bool isAvailable() const {
        return available;
    }

    //to book the room
    void bookRoom() {
        available = false;
    }

    //to free up the room
    void freeRoom() {
        available = true;
    }

    //to display room details
    void display() const {
        cout << "Room ID: " << roomId << " | Status: " << (available ? "Available" : "Occupied") << endl;
    }

    //to write room details to a file
    void writeToFile(ofstream& fout) const {
        fout << roomId << " " << (available ? "Available" : "Occupied") << endl;
    }
};

// Student class to represent each student
class Student {
private:
    string name;
    int rollNumber;
    Room* assignedRoom;

public:
    // Constructor
    Student(const string& n, int roll)
	{
		name = n;
		rollNumber = roll;
		assignedRoom = nullptr;
	} 

    //to assign a room to the student
    void assignRoom(Room& room) {
        assignedRoom = &room;
        room.bookRoom();
    }

    //to free up the assigned room
    void vacateRoom() {
        if (assignedRoom) {
            assignedRoom->freeRoom();
            assignedRoom = nullptr;
        }
    }

    //to display student details
    void display() const {
        cout << "Name: " << name << " | Roll Number: " << rollNumber << " | Room ID: ";
        if (assignedRoom) {
            cout << assignedRoom->getRoomId() << endl;
        } else {
            cout << "Not assigned" << endl;
        }
    }

    //to write student details to a file
    void writeToFile(ofstream& fout) const {
        fout << name << " " << rollNumber << " ";
        if (assignedRoom) {
            fout << assignedRoom->getRoomId() << endl;
        } else {
            fout << "NA" << endl; // NA for not assigned
        }
    }

    int getRollNumber() const {
        return rollNumber;
    }

    const string& getName() const {
        return name;
    }

    Room* getAssignedRoom() const {
        return assignedRoom;
    }
};

// Hostel class to manage rooms and students
class Hostel {
private:
    vector<Room> rooms;
    vector<Student> students;
    string filename; // File name to store hostel data

public:
    // Constructor to initialize rooms and filename
    Hostel(int numRooms, const string& file) : filename(file) {
        // Create rooms
        for (int i = 1; i <= numRooms; ++i) {
            rooms.emplace_back(Room(i));
        }
    }

    //to load data from file on startup
    void loadDataFromFile() {
        ifstream fin(filename);
        if (fin) {
            string name;
            int roll, roomId;
            while (fin >> name >> roll >> roomId) {
                if (roomId != -1) {
                    Room& room = rooms[roomId - 1]; // Room IDs are 1-based in file
                    Student student(name, roll);
                    student.assignRoom(room);
                    students.push_back(student);
                }
            }
            fin.close();
            cout << "Data loaded successfully from file." << endl;
        } else {
            cout << "No existing data file found. Starting with no bookings." << endl;
        }
    }

    //to save data to file before exit
    void saveDataToFile() {
        ofstream fout(filename);
        if (fout) {
            for (const auto& student : students) {
                student.writeToFile(fout);
            }
            fout.close();
            cout << "Data saved successfully to file." << endl;
        } else {
            cout << "Error saving data to file." << endl;
        }
    }

    //to display all rooms
    void displayRooms() const {
        cout << "Available Rooms:" << endl;
        for (const auto& room : rooms) {
            if (room.isAvailable()) {
                room.display();
            }
        }
        cout << "Occupied Rooms:" << endl;
        for (const auto& room : rooms) {
            if (!room.isAvailable()) {
                room.display();
            }
        }
    }

    //to display all students and their assigned rooms
    void displayStudents() const {
        cout << "All Students:" << endl;
        for (const auto& student : students) {
            student.display();
        }
    }

    // to book a room for a student
    void bookRoom(const string& name, int rollNumber, int roomId) {
        // Check if room exists and is available
        auto it = find_if(rooms.begin(), rooms.end(), [&roomId](const Room& room) {
            return room.getRoomId() == roomId && room.isAvailable();
        });

        if (it != rooms.end()) {
            students.emplace_back(name, rollNumber);
            students.back().assignRoom(*it);
            cout << "Room " << roomId << " booked successfully for " << name << "." << endl;
        } else {
            cout << "Room " << roomId << " is either not available or does not exist." << endl;
        }
    }

    //to vacate room for a student
    void vacateRoom(int rollNumber) {
        auto it = find_if(students.begin(), students.end(), [&rollNumber](const Student& student) {
            return student.getRollNumber() == rollNumber;
        });

        if (it != students.end()) {
            it->vacateRoom();
            students.erase(it);
            cout << "Room vacated successfully for roll number " << rollNumber << "." << endl;
        } else {
            cout << "Student with roll number " << rollNumber << " not found." << endl;
        }
    }

    //to search for a room by student name
    void searchRoomByStudentName(const string& name) const {
        auto it = find_if(students.begin(), students.end(), [&name](const Student& student) {
            return student.getName() == name;
        });

        if (it != students.end()) {
            cout << "Student found:" << endl;
            it->display();
        } else {
            cout << "Student with name '" << name << "' not found." << endl;
        }
    }
};

int main() {
    Hostel hostel(50, "hostel_data.txt"); // Create a hostel with 50 rooms and data file

    hostel.loadDataFromFile(); // Load existing data from file on startup

    int choice;
    do {
        cout << "\----------## Hostel Accommodation System ##----------" << endl;
        cout << "1. Display Available Rooms" << endl;
        cout << "2. Display All Students and Rooms" << endl;
        cout << "3. Book a Room" << endl;
        cout << "4. Vacate a Room" << endl;
        cout << "5. Search Room by Student Name" << endl;
        cout << "6. Exit" << endl;
        cout << "--------------------------------------------------------" <<endl;
        cout << "Enter your choice (1-6): ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                hostel.displayRooms();
                break;
            case 2:
                hostel.displayStudents();
                break;
            case 3: {
                string name;
                int rollNumber, roomId;
                cout << "Enter student's name: ";
                getline(cin, name);
                cout << "Enter student's roll number: ";
                cin >> rollNumber;
                cout << "Enter room ID to book: ";
                cin >> roomId;
                hostel.bookRoom(name, rollNumber, roomId);
                break;
            }
            case 4: {
                int rollNumber;
                cout << "Enter student's roll number to vacate room: ";
                cin >> rollNumber;
                hostel.vacateRoom(rollNumber);
                break;
            }
            case 5: {
                string name;
                cout << "Enter student's name to search: ";
                getline(cin, name);
                hostel.searchRoomByStudentName(name);
                break;
            }
            case 6:
                hostel.saveDataToFile(); // Save data to file before exit
                exit(0);
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 6." << endl;
        }
    } while (choice != 6);

    return 0;
}
