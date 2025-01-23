#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
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
    Student() {}
    Student(string fName, string lName, string courseName, string studentGrade);
    long getStudentID() { return studentID; }
    string getFirstName() { return firstName; }
    string getLastName() { return lastName; }
    string getCourse() { return course; }
    string getGrade() { return grade; }
    void setGrade(string newGrade) { grade = newGrade; }
    static void setLastStudentID(long id) { nextStudentID = id; }
    static long getLastStudentID() { return nextStudentID; }

    friend ofstream& operator<<(ofstream& ofs, const Student& s);
    friend ifstream& operator>>(ifstream& ifs, Student& s);
    friend ostream& operator<<(ostream& os, const Student& s);
};

long Student::nextStudentID = 0;

Student::Student(string fName, string lName, string courseName, string studentGrade) {
    nextStudentID++;
    studentID = nextStudentID;
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
    map<long, Student> students;

    bool isValidGrade(string grade) {
        vector<string> validGrades = {"A+", "A", "A-", "B+", "B", "B-", "C+", "C", "C-", "D+", "D", "D-", "F"};
        transform(grade.begin(), grade.end(), grade.begin(), ::toupper);
        return find(validGrades.begin(), validGrades.end(), grade) != validGrades.end();
    }

public:
    StudentManagementSystem();
    Student addStudent(string firstName, string lastName, string course, string grade);
    Student getStudentDetails(long studentID);
    void updateStudentGrade(long studentID, string grade);
    void deleteStudent(long studentID);
    void displayAllStudents();
    ~StudentManagementSystem();
};

StudentManagementSystem::StudentManagementSystem() {
    ifstream infile("StudentData.dat");
    if (!infile) {
        return;
    }

    Student s;
    while (infile >> s) {
        students[s.getStudentID()] = s;
    }

    if (!students.empty()) {
        Student::setLastStudentID(students.rbegin()->first);
    }

    infile.close();
}

Student StudentManagementSystem::addStudent(string firstName, string lastName, string course, string grade) {
    if (!isValidGrade(grade)) {
        throw runtime_error("Invalid grade entered!");
    }
    Student s(firstName, lastName, course, grade);
    students[s.getStudentID()] = s;

    ofstream outfile("StudentData.dat", ios::app);
    outfile << s;
    outfile.close();

    return s;
}

Student StudentManagementSystem::getStudentDetails(long studentID) {
    if (students.find(studentID) != students.end()) {
        return students[studentID];
    } else {
        throw runtime_error("Student ID not found!");
    }
}

void StudentManagementSystem::updateStudentGrade(long studentID, string grade) {
    if (!isValidGrade(grade)) {
        throw runtime_error("Invalid grade entered!");
    }
    if (students.find(studentID) != students.end()) {
        students[studentID].setGrade(grade);
    } else {
        throw runtime_error("Student ID not found!");
    }

    ofstream outfile("StudentData.dat", ios::trunc);
    for (const auto& entry : students) {
        outfile << entry.second;
    }
    outfile.close();
}

void StudentManagementSystem::deleteStudent(long studentID) {
    if (students.find(studentID) != students.end()) {
        students.erase(studentID);
        ofstream outfile("StudentData.dat", ios::trunc);
        for (const auto& entry : students) {
            outfile << entry.second;
        }
        outfile.close();
        cout << "Student with ID " << studentID << " deleted successfully." << endl;
    } else {
        throw runtime_error("Student ID not found!");
    }
}

void StudentManagementSystem::displayAllStudents() {
    for (const auto& entry : students) {
        cout << entry.second << endl;
    }
}

StudentManagementSystem::~StudentManagementSystem() {
    ofstream outfile("StudentData.dat", ios::trunc);
    for (const auto& entry : students) {
        outfile << entry.second;
    }
    outfile.close();
}

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
            try {
                Student s = sms.addStudent(firstName, lastName, course, grade);
                cout << "Student Added Successfully!" << endl;
                cout << s;
            } catch (runtime_error& e) {
                cout << e.what() << endl;
            }
            break;
        }
        case 2: {
            long studentID;
            cout << "Enter Student ID: ";
            cin >> studentID;
            try {
                Student s = sms.getStudentDetails(studentID);
                cout << "Student Details:" << endl;
                cout << s;
            } catch (runtime_error& e) {
                cout << e.what() << endl;
            }
            break;
        }
        case 3: {
            long studentID;
            string grade;
            cout << "Enter Student ID: ";
            cin >> studentID;
            cout << "Enter New Grade (e.g., A+, A, B-): ";
            cin >> grade;
            try {
                sms.updateStudentGrade(studentID, grade);
                cout << "Grade Updated Successfully!" << endl;
            } catch (runtime_error& e) {
                cout << e.what() << endl;
            }
            break;
        }
        case 4: {
            long studentID;
            cout << "Enter Student ID: ";
            cin >> studentID;
            try {
                sms.deleteStudent(studentID);
            } catch (runtime_error& e) {
                cout << e.what() << endl;
            }
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
    } while (choice != 6);

    return 0;
}
