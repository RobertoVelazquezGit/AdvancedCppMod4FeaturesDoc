// ============================================================================
// Lab Solution: Modern C++ Basics - Classes and STL Containers
// Complete implementation of student grade management system
// ============================================================================

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <limits>

using namespace std;

// ============================================================================
// Student class implementation - demonstrates encapsulation and validation
// ============================================================================

class Student {
private:
    int studentId;
    string name;
    vector<double> grades;

public:
    // Constructor with input validation
    Student(int id, const string& studentName) {
        if (id <= 0) {
            throw invalid_argument("Student ID must be positive");
        }
        if (studentName.empty()) {
            throw invalid_argument("Student name cannot be empty");
        }
        studentId = id;
        name = studentName;
    }

    // Add grade with validation
    bool addGrade(double grade) {
        if (grade < 0.0 || grade > 100.0) {
            return false;
        }
        grades.push_back(grade);
        return true;
    }

    // Calculate average using STL algorithm
    double calculateAverage() const {
        if (grades.empty()) {
            return 0.0;
        }
        double sum = accumulate(grades.begin(), grades.end(), 0.0);
        return sum / grades.size();
    }

    // Const getter methods
    int getId() const {
        return studentId;
    }

    const string& getName() const {
        return name;
    }

    const vector<double>& getGrades() const {
        return grades;
    }
};

// ============================================================================
// GradeManager class implementation - demonstrates STL container usage
// ============================================================================

class GradeManager {
private:
    // in a map keys are unique, and are kept ordered automatically, so we don't need to sort them manually   
    map<int, Student> students;

public:
    // Add student with duplicate checking
    bool addStudent(int id, const string& name) {
        // Check if student already exists
        if (students.find(id) != students.end()) {
            return false;
        }

        try {
			students.emplace(id, Student(id, name));  // emplace beacuse students is a map not a vector  
            return true;
        }
        catch (const invalid_argument& e) {
            (void)e;
            return false;
        }
    }

    // Record grade for existing student
    bool recordGrade(int studentId, double grade) {
        auto it = students.find(studentId);
        if (it == students.end()) {
            return false;
        }
        return it->second.addGrade(grade);
    }

    // Get student's average
    double getStudentAverage(int studentId) const {
        auto it = students.find(studentId);
        if (it == students.end()) {
            return -1.0;
        }
        return it->second.calculateAverage();
    }

    // Display all students in formatted table
    void displayAllStudents() const {
        if (students.empty()) {
            cout << "No students in the system." << endl;
            return;
        }

        cout << "\n=== Student Grade Report ===" << endl;
        cout << left << setw(5) << "ID"
            << setw(20) << "Name"
            << setw(10) << "Grades"
            << setw(10) << "Average" << endl;
        cout << string(45, '-') << endl;

        for (const auto& pair : students) {
            const Student& student = pair.second;
            cout << left << setw(5) << student.getId()
                << setw(20) << student.getName()
                << setw(10) << student.getGrades().size()
                << fixed << setprecision(2) << setw(10)
                << student.calculateAverage() << endl;
        }
        cout << endl;
    }

    // Calculate class average
    double getClassAverage() const {
        if (students.empty()) {
            return 0.0;
        }

        double totalAverage = 0.0;
        for (const auto& pair : students) {
            totalAverage += pair.second.calculateAverage();
        }
        return totalAverage / students.size();
    }

    // Find student with highest average
    int findTopStudent() const {
        if (students.empty()) {
            return -1;
        }

        auto topStudent = max_element(students.begin(), students.end(),
            [](const auto& a, const auto& b) {
                return a.second.calculateAverage() < b.second.calculateAverage();
            });
        // Likely implemtation of nax_element:
        //auto max = students.begin();
        //for (auto it = students.begin(); it != students.end(); ++it)
        //{
        //    if (predicate(*max, *it))
        //    {
        //        max = it;
        //    }
        //}
        //return max;

        return topStudent->first;
    }

    // Helper method to check if student exists
    bool studentExists(int studentId) const {
        return students.find(studentId) != students.end();
    }

    // Get student name by ID
    string getStudentName(int studentId) const {
        auto it = students.find(studentId);
        return (it != students.end()) ? it->second.getName() : "";
    }
};

// ============================================================================
// Menu system and user interaction functions
// ============================================================================

void displayMenu() {
    cout << "\n=== Grade Management System ===" << endl;
    cout << "1. Add Student" << endl;
    cout << "2. Record Grade" << endl;
    cout << "3. View Student Average" << endl;
    cout << "4. Display All Students" << endl;
    cout << "5. View Class Average" << endl;
    cout << "6. Find Top Student" << endl;
    cout << "7. Exit" << endl;
    cout << "Choose an option (1-7): ";
}

// Handle adding a new student
void handleAddStudent(GradeManager& manager) {
    int id;
    string name;

    cout << "Enter student ID: ";
    cin >> id;

    if (cin.fail() || id <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Please enter a valid positive student ID." << endl;
        return;
    }

    cout << "Enter student name: ";
    cin >> name;

    // Trim whitespace
    name.erase(0, name.find_first_not_of(" \t"));
    name.erase(name.find_last_not_of(" \t") + 1);

    if (name.empty()) {
        cout << "Error: Student name cannot be empty." << endl;
        return;
    }

    if (manager.addStudent(id, name)) {
        cout << "Student added successfully!" << endl;
    }
    else {
        cout << "Error: Student with ID " << id << " already exists." << endl;
    }
}

// Handle recording a grade
void handleRecordGrade(GradeManager& manager) {
    int id;
    double grade;

    cout << "Enter student ID: ";
    cin >> id;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Please enter a valid student ID." << endl;
        return;
    }

    if (!manager.studentExists(id)) {
        cout << "Error: Student with ID " << id << " not found." << endl;
        return;
    }

    cout << "Enter grade (0-100): ";
    cin >> grade;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Please enter a valid grade." << endl;
        return;
    }

    if (manager.recordGrade(id, grade)) {
        cout << "Grade recorded successfully for "
            << manager.getStudentName(id) << "!" << endl;
    }
    else {
        cout << "Error: Grade must be between 0 and 100." << endl;
    }
}

// Handle viewing a student's average
void handleViewAverage(const GradeManager& manager) {
    int id;
    cout << "Enter student ID: ";
    cin >> id;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Please enter a valid student ID." << endl;
        return;
    }

    double average = manager.getStudentAverage(id);
    if (average >= 0) {
        cout << "Average for " << manager.getStudentName(id)
            << " (ID: " << id << "): "
            << fixed << setprecision(2) << average << endl;
    }
    else {
        cout << "Error: Student with ID " << id << " not found." << endl;
    }
}

// ============================================================================
// Main program implementation
// ============================================================================

int main() {
    GradeManager manager;
    int choice;

    cout << "Welcome to the Grade Management System!" << endl;
    cout << "This system helps you manage student grades and calculate averages." << endl;

    // Add some sample data for testing
    cout << "\nAdding sample students for demonstration..." << endl;
    manager.addStudent(101, "Alice Johnson");
    manager.addStudent(102, "Bob Smith");
    manager.addStudent(103, "Carol Davis");

    manager.recordGrade(101, 85.0);
    manager.recordGrade(101, 92.0);
    manager.recordGrade(101, 78.0);

    manager.recordGrade(102, 90.0);
    manager.recordGrade(102, 95.0);

    manager.recordGrade(103, 88.0);
    manager.recordGrade(103, 91.0);
    manager.recordGrade(103, 87.0);

    cout << "Sample data added successfully!" << endl;

    do {
        displayMenu();
        cin >> choice;

        // Clear input buffer for string inputs
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            handleAddStudent(manager);
            break;
        case 2:
            handleRecordGrade(manager);
            break;
        case 3:
            handleViewAverage(manager);
            break;
        case 4:
            manager.displayAllStudents();
            break;
        case 5: {
            double classAvg = manager.getClassAverage();
            cout << "Class Average: " << fixed << setprecision(2)
                << classAvg << endl;
            break;
        }
        case 6: {
            int topStudentId = manager.findTopStudent();
            if (topStudentId != -1) {
                cout << "Top Student: " << manager.getStudentName(topStudentId)
                    << " (ID: " << topStudentId << ") with average: "
                    << fixed << setprecision(2)
                    << manager.getStudentAverage(topStudentId) << endl;
            }
            else {
                cout << "No students in the system." << endl;
            }
            break;
        }
        case 7:
            cout << "Thank you for using Grade Management System!" << endl;
            break;
        default:
            cout << "Invalid option. Please try again." << endl;
        }
    } while (choice != 7);

    return 0;
}

/*
VALIDATION CHECKLIST COMPLETED:
- Student class validates ID and name in constructor

- Student class addGrade method validates grade range (0-100)

- Student calculateAverage handles empty grades vector correctly

- GradeManager uses map<int, Student> for efficient O(log n) lookup

- GradeManager addStudent checks for duplicate IDs before adding

- All getter methods are properly const-qualified

- Menu system handles invalid input with proper error messages

- Input validation clears cin error flags and ignores invalid input

- Class average calculation handles empty student list

- Top student finder uses STL max_element with custom comparator

- Formatted output uses setw(), left alignment, and fixed precision

- Program includes comprehensive error checking throughout

COMMON MISTAKES ADDRESSED:
- Forgot to validate constructor parameters
- Not checking for duplicate student IDs
- Missing const qualifiers on getter methods
- Not handling cin errors properly
- Forgetting to clear input buffer between operations
- Division by zero when calculating averages
- Not validating grade range (0-100)
- Poor formatting in output display
*/

