#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <vector>
#include <string>
#include"Author.cpp"
using namespace std;

struct Book
{
    char ISBN[15]; // Primary key
    char Book_Title[30];
    char Author_ID[15]; // Secondary key
};

map<string, int> BookPrimaryIndex;
map<string, vector<int>> BookSecondaryIndex;
vector<AvailListEntry> BookAvailList;


void printPrimaryIndex(map<string, int>&index)
{
    for (auto x : index)
    {
        cout << x.first << "|" << x.second << endl;
    }
}

void readBooksAvailList()
{
    BookAvailList.clear();
    fstream f;
    f.open("Books.txt", ios::in | ios::out | ios::app | ios::binary);

    // parsing availList
    string line;
    char temp;
    while(!f.eof())
    {
        f>>temp;
        if (temp==']')
        {
            line += temp;
            break;
        }

        line += temp;
    }

    availListLength = line.size();
    vector<string> tokens = splitString(line.substr(1,line.size()-2), ',');
    for(int i = 0; i < tokens.size(); i+=2 )
    {
        AvailListEntry entry;
        entry.offset = stoi(tokens[i]);
        entry.size = stoi(tokens[i+1]);
        BookAvailList.push_back(entry);
    }

    //cout << AuthorAvailList.size();
    f.close();
}

int calculateBookAvailListLength()
{
    int length = 2 + BookAvailList.size()*2 - 1;

    for(int i = 0; i < BookAvailList.size(); i++)
    {
        length += to_string(BookAvailList[i].size).size() + to_string(BookAvailList[i].offset).size();
    }

    return length;
}

void writeBookAvailListTofile()
{

    fstream f;
    string fileContent = readFileToString("Books.txt");
    fileContent = fileContent.substr(availListLength, fileContent.size() - availListLength);
    f.open("Books.txt", ios::out);

    if(BookAvailList.size() == 0)
    {
        f << "[]";
        f << fileContent;
        f.close();
        return;
    }

    for(int i = 0; i < BookAvailList.size(); i++)
    {
        if(i == 0)
        {
            f << "[";

        }

        f << BookAvailList[i].offset << "," << BookAvailList[i].size;


        if(i != BookAvailList.size() -1)
        {
            f << ",";
        }
        else
        {
            f << "]";
        }
    }
    f << fileContent;
    f.close();

}

void addToBookEnd(Book &b)
{
    int recordLength = strlen(b.ISBN) + strlen(b.Book_Title) + strlen(b.Author_ID) + 2 + 2; // 2 for the delimiter and 2 for record length
    fstream f;
    f.open("Books.txt", ios::in | ios::out | ios::app | ios::binary);
    int offset = readFileToString("Books.txt").size() - availListLength;
    f.seekp(0, ios::end);
    if(recordLength < 10)
    {
        f << 0 << recordLength << b.ISBN << "|" << b.Book_Title << "|" << b.Author_ID;
    }
    else
    {
        f << recordLength << b.ISBN << "|" << b.Book_Title << "|" << b.Author_ID;
    }
    BookPrimaryIndex[b.ISBN] = offset;
    writePrimaryIndex("BookPrimaryIndex.txt", BookPrimaryIndex);
    f.close();
}

void addToBookOffset(Book& b, AvailListEntry& ave)
{
    int recordLength = strlen(b.ISBN) + strlen(b.Book_Title) + strlen(b.Author_ID) + 2 + 2; // 2 for the delimiter and 2 for record length
    fstream f;
    f.open("Books.txt", ios::in | ios::out | ios::binary);
    f.seekp(ave.offset + availListLength, ios::beg);
    cout << ave.offset << endl;
    BookPrimaryIndex[b.ISBN] = ave.offset;

    ave.offset = ave.offset + recordLength;
    ave.size = ave.size - recordLength;
    if(ave.size > 0)
    {
        BookAvailList.push_back(ave);
    }

    if(recordLength < 10)
    {
        f << 0 << recordLength << b.ISBN<< "|" << b.Book_Title << "|" << b.Author_ID;
    }
    else
    {
        f << recordLength << b.ISBN<< "|" << b.Book_Title << "|" << b.Author_ID;
    }
    writePrimaryIndex("BookPrimaryIndex.txt", BookPrimaryIndex);
    f.close();
    f.close();
}

void addBook(Book&b) {
    // loop through the avail list to see if there exists b deleted record where we can add

    int recordLength = strlen(b.ISBN) + strlen(b.Book_Title) + strlen(b.Author_ID) + 2 + 2; // 2 for the delimiter
    cout<<recordLength<<endl;
    int diff = INT_MAX;
    AvailListEntry ave;
    int index = 0;
    for (int i = 0; i < BookAvailList.size(); i++) {
        if(BookAvailList[i].size - recordLength >= 0 && BookAvailList[i].size - recordLength < diff)
        {
            ave = BookAvailList[i];
            index = i;
            diff = ave.size - recordLength;
        }
    }


    if(diff == INT_MAX)
    {
        cout << "No space left; add to the end of the file" << endl;
        addToBookEnd(b);
    }
    else
    {
        cout <<"Add to the empty space << "<< ave.size << " " <<ave.offset <<endl;
        BookAvailList.erase(BookAvailList.begin() + index);
        addToBookOffset(b, ave);
        writeBookAvailListTofile();
        availListLength = calculateBookAvailListLength();

    }


}

void deleteBook(string isbn) {

    auto it = BookPrimaryIndex.find(isbn);

    if (it != BookPrimaryIndex.end()) {
        int BookOffset = it->second;

        AvailListEntry ave;
        ave.offset = BookOffset;
//        cout<<"offset " << ave.offset << endl;

        fstream f;
        f.open("Books.txt", ios::in | ios::out | ios::binary);

        f.seekg(BookOffset + availListLength, ios::beg);
        // Read the first two characters (assuming the size is stored as a two-character string)
        char BookSizeBuffer[3];  // One extra character for null terminator
        f.read(BookSizeBuffer, sizeof (BookSizeBuffer));
        BookSizeBuffer[2] = '\0';  // Null-terminate the string
//        cout << "BookSizeBuffer: " << BookSizeBuffer << endl;
        // Convert the size to an integer
        stringstream ss(BookSizeBuffer);
        int result;
        ss >> result;
        ave.size = result;
//        cout << "Read from file: " << BookSizeBuffer << endl;
//        cout << "ave.size: " << ave.size << endl; // debug output


//        cout<<"before"<<BookAvailList.size();
        BookAvailList.push_back(ave);
//        cout << "after" << BookAvailList.size();
        writeBookAvailListTofile();
        availListLength = calculateBookAvailListLength();

        // Move back to the position before the size to overwrite with '*'
        f.seekp(BookOffset+availListLength, ios::beg);
        f.put('*');

        f.close();
        BookPrimaryIndex.erase(it);
        writePrimaryIndex("BookPrimaryIndex.txt", BookPrimaryIndex);

        cout << "Book deleted successfully" << endl;
    } else {
        cout << "Book not found" << endl;
    }
}

void updateBook() {
    cout << "Enter book isbn:" << endl;
    string ISBN;
    cin >> ISBN;
    auto it = BookPrimaryIndex.find(ISBN);

    if (it != BookPrimaryIndex.end()) {
        int bookOffset = it->second;
        fstream f;
        f.open("Books.txt", ios::in | ios::binary);

        f.seekg(bookOffset + availListLength, ios::beg);
        char bookSizeBuffer[3];
        f.read(bookSizeBuffer, sizeof(bookSizeBuffer));
        bookSizeBuffer[2] = '\0';

        char *book = new char[atoi(bookSizeBuffer) + 1];
        f.seekg(bookOffset + availListLength, ios::beg);
        f.read(book, atoi(bookSizeBuffer));
        book[atoi(bookSizeBuffer)] = '\0';

        string authorid;
        vector<string> bookData = splitString(book, '|');
        authorid = bookData[2];
//        cout << "Old Author Id: " << authorid << endl;

        delete[] book; // Free the dynamically allocated memory

        deleteBook(ISBN);

        Book b;
        strncpy(b.ISBN, ISBN.c_str(), sizeof(b.ISBN) - 1);
        b.ISBN[sizeof(b.ISBN) - 1] = '\0';  // Null-terminate the string

        cout << "Enter new book title: ";
        cin.ignore();
        cin.getline(b.Book_Title, sizeof(b.Book_Title));

        strncpy(b.Author_ID, authorid.c_str(), sizeof(b.Author_ID) - 1);
        b.Author_ID[sizeof(b.Author_ID) - 1] = '\0';  // Null-terminate the string

        addBook(b); // Add the updated author

        cout << "Book updated successfully" << endl;
    } else {
        cout << "Book not found" << endl;
    }
}

void printBook() {
    string BookISBN;
    cout << "Enter Book ISBN :";
    cin >> BookISBN;
    auto it = BookPrimaryIndex.find(BookISBN);

    if (it != BookPrimaryIndex.end()) {
        int bookOffset = it->second;
        fstream f;
        f.open("Books.txt", ios::in | ios::binary);

        availListLength=calculateBookAvailListLength();
        f.seekg(bookOffset + availListLength, ios::beg);
        char bookSizeBuffer[3];
        f.read(bookSizeBuffer, sizeof(bookSizeBuffer));
        bookSizeBuffer[2] = '\0';

        char *book = new char[atoi(bookSizeBuffer) + 1];
        f.seekg(bookOffset + availListLength, ios::beg);
        f.read(book, atoi(bookSizeBuffer));
        book[atoi(bookSizeBuffer)] = '\0';

        string authorid;
        vector<string> bookData = splitString(book, '|');
        string BookISBN = bookData[0].substr(2);  // Extract substring from the third character to the end
        string BookTitle = bookData[1];
        string authorID = bookData[2];
        cout <<  "ISBN :  " << BookISBN <<  "  Title :  " << BookTitle <<  "  Author ID :  " << authorID <<endl;

        delete[] book; // Free the dynamically allocated memory

    }
}
