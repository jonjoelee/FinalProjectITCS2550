#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;
// Define a constant welcome message, style inspired from C++ lvl 1 class last semester
const string welcome = " Welcome to the Berry Management System ";

// Define struct for books
struct Book {
    string title;
    string author;
    bool checkedOut;

    // Constructors
    Book(const string& _title, const string& _author, bool _checkedOut) : title(_title), author(_author), checkedOut(_checkedOut) {}
    Book() : title(""), author(""), checkedOut(false) {}
};

// Overload output stream operator for Book
ostream& operator<<(ostream& os, const Book& book) {
    os << "\nTitle: " << book.title << "\nAuthor: " << book.author << "\nChecked Out: " << (book.checkedOut ? "Yes" : "No") << endl;
    return os;
}

// Define base class Person
class Person {
protected:
    string firstName;
    string lastName;

public:
    // Constructor
    Person(const string& _firstName, const string& _lastName) : firstName(_firstName), lastName(_lastName) {}

    // Getters
    string getFirstName() const { return firstName; }
    string getLastName() const { return lastName; }
};

// Define a derived class Patron from Person
class Patron : public Person {
private:
    vector<Book*> checkedOutBooks;

public:
    // Constructor
    Patron(const string& _firstName, const string& _lastName) : Person(_firstName, _lastName) {}

    // Getters
    const vector<Book*>& getCheckedOutBooks() const { return checkedOutBooks; }

    // Destructor
    ~Patron() {
        for (Book* book : checkedOutBooks) {
            delete book;
        }
        checkedOutBooks.clear();
    }

    // Functions specific to Patron class
    void checkOutBook(Book* book) {
        Book* newBook = new Book(*book);
        checkedOutBooks.push_back(newBook);
    }
    void returnBook(const string& title) {
        auto it = find_if(checkedOutBooks.begin(), checkedOutBooks.end(), [&title](Book* book) {
            return book->title == title;
            });
        if (it != checkedOutBooks.end()) {
            delete* it;
            checkedOutBooks.erase(it);
        }
    }
};

// Define template class Library
template<typename BookContainer, typename PatronContainer, typename PersonContainer>
class Library {
private:
    BookContainer books;
    PatronContainer patrons;
    PersonContainer people;
    time_t now = time(0);
    struct tm* local_time = localtime(&now);
    char* date_time = asctime(local_time);

public:
    // Function to convert string to lowercase
    string lower(string toBeLower) {
        for (char& c : toBeLower) {
            c = tolower(c);
        }
        return toBeLower;
    }
    // Funciton to detect whitespace in string
    bool hasWhitespace(const string& str) {
        return any_of(str.begin(), str.end(), [](char c) { 
            if (isspace(c)) {
            cout << "Error: Whitespaces are not allowed in the name.\n";
        }
        return isspace(c); });
    }
    
    // Function to log user name with date, this mimics the librarian/user
    void logUserName(const string& firstName, const string& lastName) {
        people.push_back(Person(firstName, lastName));
        ofstream logFile("user_log.txt", ios::app);
        if (logFile.is_open()) {
            logFile << "User: " << firstName << " " << lastName << " - Date: " << date_time << "\n";
            logFile.close();
        }
        else {
            cerr << "Unable to open log file for writing.\n";
        }
    }

    // Function to search books by author
    void searchBooksByAuthor() {
        string authorFirstName, authorLastName;
        cout << "Enter author first name: ";
        getline(cin, authorFirstName);
        if (hasWhitespace(authorFirstName)) { return; }
        cout << "Enter author last name: ";
        getline(cin, authorLastName);
        if (hasWhitespace(authorLastName)) { return; }

        cout << "\nRESULTS:\nBooks by author " << authorFirstName << " " << authorLastName << ":\n";
        for (const Book& book : books) {
            if (lower(book.author) == lower(authorFirstName) + " " + lower(authorLastName)) {
                cout << "- " << book.title << " (";
                if (book.checkedOut) {
                    cout << "Checked out)\n";
                }
                else {
                    cout << "Available)\n";
                }
            }
        }
    }

    // Function to search for a book by title
    void searchBookByTitle() {
        string title;
        cout << "Enter book title: ";
        getline(cin, title);

        for (const Book& book : books) {
            if (lower(book.title) == lower(title)) {
                cout << "\nRESULTS:\n-Book: " << book.title << " by " << book.author << " (";
                if (book.checkedOut) {
                    cout << "Checked out)\n";
                }
                else {
                    cout << "Available)\n";
                }
                return;
            }
        }
        cout << "Book not found.\n";
    }
    
    // Function to add a book
    void addBook() {
        string title, authorFirstName, authorLastName;
        cout << "Enter the title of the book: ";
        getline(cin, title);
        cout << "Enter the first name of the author of the book: ";
        getline(cin, authorFirstName);
        if (hasWhitespace(authorFirstName)) { return; };

        cout << "Enter the last name of the author of the book: ";
        getline(cin, authorLastName);
        if (hasWhitespace(authorLastName)) { return; }

        Book newBook(title, authorFirstName + " " + authorLastName, false);
        books.push_back(newBook);
        cout << newBook << "\nBook added to the library.\n";
    }

    // Function to remove a book
    void removeBook() {
        string title, authorFirstName, authorLastName;
        cout << "*Case sensitive for safety\n";
        cout << "Enter the title of the book to remove: ";
        getline(cin, title);
        cout << "Enter the first name of the author of the book to remove: ";
        getline(cin, authorFirstName);
        if (hasWhitespace(authorFirstName)) { return; };

        cout << "Enter the last name of the author of the book to remove: ";
        getline(cin, authorLastName);
        if (hasWhitespace(authorLastName)) { return; };

        cout << "\nTitle: " << title << ", Author: " << authorFirstName << " " << authorLastName << endl;

        auto it = find_if(books.begin(), books.end(), [&title, &authorFirstName, &authorLastName](const Book& book) {
            return (book.title == title && book.author == authorFirstName + " " + authorLastName);
            });

        if (it != books.end()) {
            books.erase(it);
            cout << "Has been successfully removed from the library.\n";
        }
        else {
            cout << "Book not found in the library.\n";
        }

    }

    // Function to check out a book
    void checkOutBook() {
        string patronFirstName, patronLastName, bookTitle;
        cout << "Enter patron first name: ";
        cin >> patronFirstName;
        cout << "Enter patron last name: ";
        cin >> patronLastName;
        cout << "Enter book title to check out: ";
        cin.ignore();
        getline(cin, bookTitle);

        bool patronFound = false;
        for (Patron& patron : patrons) {
            if (lower(patron.getFirstName()) == lower(patronFirstName) && lower(patron.getLastName()) == lower(patronLastName)) {
                patronFound = true;
                for (Book& book : books) {
                    if (lower(book.title) == lower(bookTitle)) {
                        if (!book.checkedOut) {
                            book.checkedOut = true;
                            patron.checkOutBook(&book);
                            cout << book.title << " has been checked out by " << patronFirstName << " " << patronLastName << endl;
                            return;
                        }
                        else {
                            cout << "Book is already checked out.\n";
                            return;
                        }
                    }
                }
                cout << "Book not found.\n";
                return;
            }
        }
        cout << "Patron not found.\n";
    }

    // Function to return a book
    void returnBook() {
        string patronFirstName, patronLastName, bookTitle;
        cout << "Enter patron first name: ";
        cin >> patronFirstName;
        cout << "Enter patron last name: ";
        cin >> patronLastName;
        cout << "Enter book title to return: ";
        cin.ignore();
        getline(cin, bookTitle);

        bool patronFound = false;
        for (Patron& patron : patrons) {
            if (patron.getFirstName() == patronFirstName && patron.getLastName() == patronLastName) {
                patronFound = true;
                for (Book& book : books) {
                    if (lower(book.title) == lower(bookTitle)) {
                        if (book.checkedOut) {
                            book.checkedOut = false;
                            patron.returnBook(bookTitle);
                            cout << book.title << " has been returned by " << patronFirstName << " " << patronLastName << endl;
                            return;
                        }
                        else {
                            cout << "Book is not checked out.\n";
                            return;
                        }
                    }
                }
                cout << "Book not found.\n";
                return;
            }
        }
        cout << "Patron not found.\n";
    }

    // Function to sort books by title
    void sortBooksByTitle() {
        sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
            return a.title < b.title;
            });
    }

    // Function to sort books by author
    void sortBooksByAuthor() {
        sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
            return a.author < b.author;
            });
    }

    // Function to prompt user to sort books by title or author
    void promptSortBy() {
        char choice;
        cout << "Sort by (T)itle or (A)uthor? ";
        cin >> choice;
        switch (toupper(choice)) {
        case 'T':
            sortBooksByTitle();
            break;
        case 'A':
            sortBooksByAuthor();
            break;
        default:
            cout << "Invalid choice. Sorting by title by default.\n";
            sortBooksByTitle();
            break;
        }
    }

    // Function to print all books
    void printAllBooks() {
        cout << "\nList of Books:\n";
        for (const Book& book : books) {
            cout << "- " << book.title << " by " << book.author << endl;
        }
    } 

    // Function to add a patron
    void addPatron() {
        string firstName, lastName;
        cout << "Enter first name of the patron to add: ";
        getline(cin, firstName);
        if (hasWhitespace(firstName)) {
            return; 
        }
        cout << "Enter last name of the patron to add: ";
        getline(cin, lastName);
        if (hasWhitespace(lastName)) {
            return; // Abort adding patron
        }
        patrons.push_back(Patron(firstName, lastName));
        cout << "\nPatron added: " << firstName << " " << lastName << "\n";
    }

    // Function to remove a patron
    void removePatron() {
        string firstName, lastName;
        cout << "Enter first name of the patron to remove: ";
        getline(cin, firstName);
        if (hasWhitespace(firstName)) {
            return;
        }
        cout << "Enter last name of the patron to remove: ";
        getline(cin, lastName);
        if (hasWhitespace(lastName)) {
            return;
        }

        auto it = find_if(patrons.begin(), patrons.end(), [&firstName, &lastName](const Patron& patron) {
            return patron.getFirstName() == firstName && patron.getLastName() == lastName;
            });
        if (it != patrons.end()) {
            patrons.erase(it);
            cout << "\nPatron removed: " << firstName << " " << lastName << "\n";
        }else {
            cout << "\nPatron not found.\n";
        }
    }

    // Function to display books checked out by a patron
    void displayBooksCheckedOutByPatron() {
        string firstName, lastName;
        cout << "Enter patron first name: ";
        cin >> firstName;
        cout << "Enter patron last name: ";
        cin >> lastName;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        bool found = false;
        for (const Patron& patron : patrons) {
            if (lower(patron.getFirstName()) == lower(firstName) && lower(patron.getLastName()) == lower(lastName)) {
                found = true;
                const vector<Book*>& checkedOutBooks = patron.getCheckedOutBooks();
                if (!checkedOutBooks.empty()) {
                    cout << "\nBooks checked out by " << firstName << " " << lastName << ":\n";
                    for (const Book* book : checkedOutBooks) {
                        cout << "- " << book->title << " by " << book->author << "\n";
                    }
                }else {
                    cout << "\nNo books checked out by " << firstName << " " << lastName << "\n";
                }
                break;
            }
        }
        if (!found) {
            cout << "\nPatron not found.\n";
        }
    }

    // Function to print all patrons
    void printAllPatrons() {
        cout << "\nList of Patrons:\n";
        for (const Patron& patron : patrons) {
            cout << "- " << patron.getFirstName() << " " << patron.getLastName() << "\n";
        }
    }

    // Function to write books to a JSON file
    void writeToLogFile() {
        ofstream outFile("booksLogTo.json");
        if (outFile.is_open()) {
            json jsonData = json::array();
            for (const Book& book : books) {
                jsonData.push_back({
                    {"Author", book.author},
                    {"Title", book.title},
                    {"CheckedOut", book.checkedOut}
                    });
            }
            outFile << jsonData.dump(4);
            outFile.close();
            cout << "\nBooks written to file.\n\n";
        }
        else {
            cerr << "Unable to open file for writing.\n";
        }
    }

    // Function to read books from a JSON file
    void readFromFile() {
        ifstream inFile("books.json");
        try {
            json jsonData;
            inFile >> jsonData;
            inFile.close();

            for (const auto& bookData : jsonData) {
                string title = bookData["Title"];
                string author = bookData["Author"];
                bool checkedOut = bookData["CheckedOut"];
                books.push_back(Book{ title, author, checkedOut });
            }
            cout << "\n-Books read from file.\n\n";
        }
        catch (const exception& e) {
            cerr << "An error occurred while reading from file: " << e.what() << endl;
        }
    }

    void addPatronsForTesting(string first, string last) {
        patrons.push_back(Patron(first, last));
    }

    void checkOutBookForTestPatrons(string first,string last,string title) {
        bool patronFound = false;
        for (Patron& patron : patrons) {
            if (lower(patron.getFirstName()) == lower(first) && lower(patron.getLastName()) == lower(last)) {
                patronFound = true;
                for (Book& book : books) {
                    if (lower(book.title) == lower(title)) {
                        if (!book.checkedOut) {
                            book.checkedOut = true;
                            patron.checkOutBook(&book);
                            return;
                        }else {return; }
                    }
                }
                return;
            }
        }
    }

};


int main() {
    Library<vector<Book>, vector<Patron>, vector<Person>> library;
    string firstName, lastName;

    // Welcome message
    cout << right << setw(57) << setfill('*') << "*\n";
    cout << right << setw(57) << setfill('*') << "*\n";
    cout << right << setw(48) << setfill('*') << welcome << right << setw(9) << setfill('*');
    cout << "\n" << right << setw(56) << setfill('*') << "*";
    cout << "\n" << right << setw(58) << setfill('*') << "*\n\n";

    // Prompt user to enter their name
    cout << "Enter your first name: ";
    cin >> firstName;
    cout << "Enter your last name: ";
    cin >> lastName;
    cin.ignore();
    // Log user(librarian) with date
    library.logUserName(firstName, lastName);

    //For submission only, normally called by case 'R' and user types file to read from
    library.readFromFile();


    /*For submission only, Added this while double checking the rubric before submission bc I noticed the comment not to make you add any files,
    I assumed that meant test data too. I had planned for the user(librarian) to add patrons and for none to exist until the librarian added them.
    I added these test users so all the functions are usable immediately after compiling the program.
    The ones effected by this if I didnt add the test data were checkout book, return book, view all patrons, search patron and then view all books checked out to that patron.
    */
    library.addPatronsForTesting("Sarah", "Lee");
    library.addPatronsForTesting("Sam", "Dunfey");
    library.addPatronsForTesting("James", "Jones");
    library.addPatronsForTesting("Candace", "Baker");
    library.checkOutBookForTestPatrons("Sarah", "Lee", "Python Programming");
    library.checkOutBookForTestPatrons("Sam", "Dunfey", "Web Development Crash Course");
    library.checkOutBookForTestPatrons("James", "Jones", "The Art of Programming VOL2");
    library.checkOutBookForTestPatrons("James", "Jones", "Blockchain Technology Explained");
    library.checkOutBookForTestPatrons("Candace", "Baker", "Artificial Intelligence Basics");
    //One more note, the books.json does not have any books checked out.
    //If you trigger the writeToLogFile function with case W, they will appear checked out.


    string input;
    char choice = 0;
    while (choice != 'X') {
        cout << "\nPress L to print List of Commands:\n";
        cout << "Command: ";
        getline(cin, input);

        if (input.length() > 1) {
            cout << "Error: User has entered more than one character\nPlease only enter one character.";
            continue;
        }
        // Convert input to uppercase
        choice = toupper(input[0]);

        // Switch case for different commands
        switch (choice) {
        case 'L': {
            cout << "\nList of Commands:\n";
            cout << "Press A to search for books by author\n";
            cout << "Press B to search for a specific book title\n";
            cout << "Press I to add a new book\n";
            cout << "Press M to remove a books\n";
            cout << "Press C to check out a book\n";
            cout << "Press U to return a book\n";
            cout << "Press S to sort books by title or author\n";
            cout << "Press V to print a list of all books\n\n";

            cout << "Press P to search for a patron and display their checked out books\n";
            cout << "Press N to add new patron\n";
            cout << "Press T to display all patrons\n";
            cout << "Press D to delete a patron\n\n";

            cout << "Press R to read books from file\n";
            cout << "Press W to write books to file\n";
            cout << "Press X to exit program\n";
            break;
        }
        case 'A': {
            try {
                library.searchBooksByAuthor();
            }
            catch (const exception& e) {
                cerr << "An error occurred: " << e.what() << endl;
            }
            break;
        }
        case 'B': {
            library.searchBookByTitle();
            break;
        }

        case 'I': {
            library.addBook();
            break;
        }
        case 'M': {
            library.removeBook();
            break;
        }
        case 'C': {
            library.checkOutBook();
            break;
        }
        case 'U': {
            library.returnBook();
            break;
        }
        case 'S': {
            library.promptSortBy();
            library.printAllBooks();
            break;
        }
        case 'V': {
            library.printAllBooks();
            break;
        }


        case 'P': {       
            library.displayBooksCheckedOutByPatron();
            break;
        }
        case 'N': {    
            library.addPatron();
            break;
        }


        case 'D': {    
            library.removePatron();
            break;
        }

        case 'R': {
            /*
            string fileName;
            cout << "Enter file name (For this example please type books.json): ";
            cin >> fileName;
            */
            library.readFromFile();
            break;
        }
        case 'W': {
            library.writeToLogFile();
            break;
        }
        case 'X': {
            cout << "\nExiting program...\n\n";
            break;
        }
        case 'T': {
            library.printAllPatrons();
            break;
        }
        default: {
            cout << "Invalid choice. Please try again";
            break;
        }
        }
    }
}
