#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <memory>
#include <stdexcept>

using namespace std;

class Student {
private:
    long studentID;
    string firstName;
    string lastName;
    string course;
    string grade;
    static long nextStudentID;

public:
    Student() : studentID(0) {}
    Student(string fName, string lName, string courseName, string studentGrade);
    long getStudentID() const { return studentID; }
    string getFirstName() const { return firstName; }
    string getLastName() const { return lastName; }
    string getCourse() const { return course; }
    string getGrade() const { return grade; }
    void setGrade(string newGrade) { grade = newGrade; }
    static void setLastStudentID(long id) { nextStudentID = id; }
    static long getLastStudentID() { return nextStudentID; }

    friend ofstream& operator<<(ofstream& ofs, const Student& s);
    friend ifstream& operator>>(ifstream& ifs, Student& s);
    friend ostream& operator<<(ostream& os, const Student& s);
};

long Student::nextStudentID = 0;

Student::Student(string fName, string lName, string courseName, string studentGrade) {
    studentID = ++nextStudentID;
    firstName = fName;
    lastName = lName;
    course = courseName;
    transform(studentGrade.begin(), studentGrade.end(), studentGrade.begin(), ::toupper);
    grade = studentGrade;
}

ofstream& operator<<(ofstream& ofs, const Student& s) {
    ofs << s.studentID << endl;
    ofs << s.firstName << endl;
    ofs << s.lastName << endl;
    ofs << s.course << endl;
    ofs << s.grade << endl;
    return ofs;
}

ifstream& operator>>(ifstream& ifs, Student& s) {
    if (ifs >> s.studentID) {
        ifs.ignore();
        getline(ifs, s.firstName);
        getline(ifs, s.lastName);
        getline(ifs, s.course);
        getline(ifs, s.grade);
    }
    return ifs;
}

ostream& operator<<(ostream& os, const Student& s) {
    os << "Student ID: " << s.studentID << endl;
    os << "First Name: " << s.firstName << endl;
    os << "Last Name: " << s.lastName << endl;
    os << "Course: " << s.course << endl;
    os << "Grade: " << s.grade << endl;
    return os;
}

class StudentManagementSystem {
private:
    map<long, unique_ptr<Student>> students;

    bool isValidGrade(const string& grade) {
        vector<string> validGrades = {"A+", "A", "A-", "B+", "B", "B-", "C+", "C", "C-", "D+", "D", "D-", "F"};
        string upperGrade = grade;
        transform(upperGrade.begin(), upperGrade.end(), upperGrade.begin(), ::toupper);
        return find(validGrades.begin(), validGrades.end(), upperGrade) != validGrades.end();
    }

    void saveToFile() {
        ofstream outfile("StudentDb.data", ios::trunc);
        if (!outfile) {
            throw runtime_error("Failed to open file for writing!");
        }
        for (const auto& entry : students) {
            outfile << *(entry.second);
        }
        outfile.close();
    }

public:
    StudentManagementSystem() {
        ifstream infile("StudentDb.data");
        if (!infile) {
            return;
        }

        Student temp;
        while (infile >> temp) {
            students[temp.getStudentID()] = make_unique<Student>(temp);
        }

        if (!students.empty()) {
            Student::setLastStudentID(students.rbegin()->first);
        }

        infile.close();
    }

    Student* addStudent(string firstName, string lastName, string course, string grade) {
        if (!isValidGrade(grade)) {
            throw runtime_error("Invalid grade entered!");
        }
        auto s = make_unique<Student>(firstName, lastName, course, grade);
        Student* ptr = s.get();
        students[s->getStudentID()] = move(s);
        saveToFile();
        return ptr;
    }

    Student* getStudentDetails(long studentID) {
        if (students.find(studentID) != students.end()) {
            return students[studentID].get();
        } else {
            throw runtime_error("Student ID not found!");
        }
    }

    void updateStudentGrade(long studentID, string grade) {
        if (!isValidGrade(grade)) {
            throw runtime_error("Invalid grade entered!");
        }
        if (students.find(studentID) != students.end()) {
            students[studentID]->setGrade(grade);
            saveToFile();
        } else {
            throw runtime_error("Student ID not found!");
        }
    }

    void deleteStudent(long studentID) {
        if (students.find(studentID) != students.end()) {
            students.erase(studentID);
            saveToFile();
            cout << "Student with ID " << studentID << " deleted successfully." << endl;
        } else {
            throw runtime_error("Student ID not found!");
        }
    }

    void displayAllStudents() {
        for (const auto& entry : students) {
            cout << *(entry.second) << endl;
        }
    }

    ~StudentManagementSystem() {
        saveToFile();
    }
};

int main() {
    StudentManagementSystem sms;
    int choice;
    do {
        cout << "\n*** Student Database Management System ***" << endl;
        cout << "1. Add Student Record" << endl;
        cout << "2. Retrieve Student Details" << endl;
        cout << "3. Update Student Grade" << endl;
        cout << "4. Delete Student Record" << endl;
        cout << "5. Display All Students" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        try {
            switch (choice) {
            case 1: {
                string firstName, lastName, course, grade;
                cout << "Enter First Name: ";
                cin >> firstName;
                cout << "Enter Last Name: ";
                cin >> lastName;
                cout << "Enter Course: ";
                cin >> course;
                cout << "Enter Grade (e.g., A+, A, B-): ";
                cin >> grade;
                Student* s = sms.addStudent(firstName, lastName, course, grade);
                cout << "Student Added Successfully!" << endl;
                cout << *s;
                break;
            }
            case 2: {
                long studentID;
                cout << "Enter Student ID: ";
                cin >> studentID;
                Student* s = sms.getStudentDetails(studentID);
                cout << "Student Details:" << endl;
                cout << *s;
                break;
            }
            case 3: {
                long studentID;
                string grade;
                cout << "Enter Student ID: ";
                cin >> studentID;
                cout << "Enter New Grade (e.g., A+, A, B-): ";
                cin >> grade;
                sms.updateStudentGrade(studentID, grade);
                cout << "Grade Updated Successfully!" << endl;
                break;
            }
            case 4: {
                long studentID;
                cout << "Enter Student ID: ";
                cin >> studentID;
                sms.deleteStudent(studentID);
                break;
            }
            case 5:
                cout << "All Students:" << endl;
                sms.displayAllStudents();
                break;
            case 6:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice! Try again." << endl;
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    } while (choice != 6);

    return 0;
}
