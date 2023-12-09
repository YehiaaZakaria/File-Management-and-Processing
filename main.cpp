#include <iostream>
#include <bits/stdc++.h>
#include <string>
//#include"Author.cpp"
#include "Book.cpp"
using namespace std;


int main() {

    loadPrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);
    loadPrimaryIndex("BookPrimaryIndex.txt",BookPrimaryIndex);
    loadSecondaryIndex("AuthorSecondaryIndex.txt", AuthorSecondaryIndex);
    loadSecondaryIndex("BookSecondaryIndex.txt",BookSecondaryIndex);
    readAuthorsAvailList();
    readBooksAvailList();
    calculateBookAvailListLength();
    calculateAvailListLength();
    int choice;

    do
    {
        cout << "Menu:\n";
        cout << "1. Add New Author\n";
        cout << "2. Add New Book\n";
        cout << "3. Update Author Name (Author ID)\n";
        cout << "4. Update Book Title (ISBN)\n";
        cout << "5. Delete Book (ISBN)\n";
        cout << "6. Delete Author (Author ID)\n";
        cout << "7. Print Author (Author ID)\n";
        cout << "8. Print Book (ISBN)\n";
        cout << "9. Write Query\n";
        cout << "10. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
//                loadPrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);
                //input
                Author a;
                cout << "Enter author id:";
                cin >> a.Author_ID;
                cin.ignore();
                {
                    auto it = AuthorPrimaryIndex.find(a.Author_ID);

                    if (it != AuthorPrimaryIndex.end()) {
                        cout << "Author already exists" << endl;
                        break;
                    } else {
                        cout << "Enter author name:";
                        cin.getline(a.Author_Name, sizeof(a.Author_Name));

                        cout << "Enter author address:";
                        cin.getline(a.Address, sizeof(a.Address));
                    }
                }
                readAuthorsAvailList();
                addAuthor(a);
                break;

            case 2:
                Book b;
                cout << "Enter book isbn:";
                cin >> b.ISBN;
                cin.ignore();
                {
                    auto it = BookPrimaryIndex.find(b.ISBN);

                    if (it != BookPrimaryIndex.end()) {
                        cout << "Book already exists" << endl;
                        break;
                    } else {
                        cout << "Enter Book title:";
                        cin.getline(b.Book_Title, sizeof(b.Book_Title));

                        cout << "Enter author ID:";
                        cin.getline(b.Author_ID, sizeof(b.Author_ID));
                        loadPrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);
                        auto authorIt = AuthorPrimaryIndex.find(b.Author_ID);
                        if (authorIt == AuthorPrimaryIndex.end()) {
                            cout << "Author with ID " << b.Author_ID << " does not exist" << endl;
                            break;
                        }

                    }
                }
                readBooksAvailList();
//                loadPrimaryIndex("BookPrimaryIndex.txt",BookPrimaryIndex);
                addBook(b);
                break;

            case 3:
                readAuthorsAvailList();
//                loadPrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);
                updateAuthor();
                break;

            case 4:
                readBooksAvailList();
//                loadPrimaryIndex("BookPrimaryIndex.txt",BookPrimaryIndex);
                updateBook();
                break;

            case 5:
                cout << "Enter Book isbn:" << endl;
                {
                    string isbn;
                    cin >> isbn;
                    readBooksAvailList();
//                    loadPrimaryIndex("BookPrimaryIndex.txt", BookPrimaryIndex);
                    deleteBook(isbn);
                }
                break;
            case 6:
                cout << "Enter author id:" << endl;
                {
                    string authorID;
                    cin >> authorID;
                    auto it = AuthorPrimaryIndex.find(authorID);
                    if (it == AuthorPrimaryIndex.end()) {
                        cout << "Author not found" << endl;
                        break;
                    }

                    readAuthorsAvailList();
//                    loadPrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);
                    deleteAuthor(authorID);
                }
                break;

            case 7:
                readAuthorsAvailList();
//                loadPrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);
                printAuthor();
                break;
            case 8:
                readBooksAvailList();
//                loadPrimaryIndex("BookPrimaryIndex.txt", BookPrimaryIndex);
                printBook();
                break;
            case 9:
                readAuthorsAvailList();
                readBooksAvailList();
                writeQueryResult();
                break;
            case 10:
                cout << "Exiting program.\n";
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 10 .\n";
                break;
        }

    } while (choice != 10);
    return 0;
}

