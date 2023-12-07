#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <vector>
#include <string>
using namespace std;

struct Author
{
    char Author_ID[15]; // Primary key
    char Author_Name[30];
    char Address[30];
};

struct AvailListEntry
{
    int offset;
    int size;
};

map<string, int> AuthorPrimaryIndex;
map<string, vector<string>> AuthorSecondaryIndex;
vector<AvailListEntry> AuthorAvailList;
int availListLength;

// helper functions.
string readFileToString(const string& fileName) {
    std::ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return "";
    }

    std::ostringstream fileContent;
    fileContent << inputFile.rdbuf(); // Read the entire file into the stringstream

    inputFile.close(); // Close the file

    return fileContent.str(); // Convert stringstream to string and return
}

vector<string> splitString(const string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(input);
    std::string token;

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

void loadPrimaryIndex(string IndexFileName, map<string, int> &Index)
{
    fstream f;
    f.open(IndexFileName, ios::in | ios::out | ios::app | ios::binary);
    string line;
    while (getline(f, line))
    {
        char delimiter = '|';
        int loc = 0;
        for(int i = 0; i < line.size(); i++)
        {
            if(line[i] == delimiter)
            {
                loc = i;
                break;
            }
        }
        Index[line.substr(0, loc)] = stoi(line.substr(loc + 1, line.size()));
    }
    f.close();
};

void printSecondaryIndex(map<string, vector<string>>&index)
{
    for (const auto &entry : index) {
        const string &authorName = entry.first;
        const vector<string> &authorIDs = entry.second;

        cout << "Author name:" << authorName << endl;
        cout << "Author IDs:" << endl;
        for (const string &authorID: authorIDs) {
            cout << authorID << endl;
        }
        cout << endl;
    }
}

void loadSecondaryIndex(string IndexFileName, map<string, vector<string>> &Index)
{

    Index.clear();
    cout<<"inside load"<<endl;
    fstream f;
    f.open(IndexFileName, ios::in | ios::out | ios::app | ios::binary);
    string line;
    while (getline(f, line))
    {
        char delimiter = '|';

        int loc = 0;
        for(int i = 0; i < line.size(); i++)
        {
            if(line[i] == delimiter)
            {
                loc = i;
                break;
            }
        }
        vector<string>v = splitString(line.substr(loc + 1, line.size()), ',');
        for(int i = 0; i < v.size(); i++)
        {
            Index[line.substr(0, loc)].push_back(v[i]);
        }

//        cout<< Index.begin()->first;
//        cout<<line.substr(0,loc)<<endl;
//
//        cout<<line.substr(loc + 1, line.size())<<endl;
    }
    f.close();
};

void writePrimaryIndex(string IndexFileName, map<string, int> &Index)
{
    ofstream f(IndexFileName);
    if (!f)
    {
        cout << "File not found" << endl;
        return;
    }
    for (auto x : Index)
    {

        f << x.first << "|" << x.second << endl;
    }
    f.close();
};

void writeSecondaryIndex(string IndexFileName, map<string, vector<string>> &Index)
{
    fstream f;
    f.open(IndexFileName, ios::out|ios::binary);
    if (!f)
    {
        cout << "File not found" << endl;
        return;
    }
    for (const auto &entry : Index) {
        const string &authorName = entry.first;
        const vector<string> &authorIDs = entry.second;

        f << authorName << "|";
        for (size_t i = 0; i < authorIDs.size(); ++i) {
            f << authorIDs[i];
            if (i < authorIDs.size() - 1) {
                f << ",";
            }
        }
        f << endl;
    }
    f.close();
}


void readAuthorsAvailList()
{
    AuthorAvailList.clear();
    fstream f;
    f.open("Authors.txt", ios::in | ios::out | ios::app | ios::binary);

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
        AuthorAvailList.push_back(entry);
    }

    f.close();
}

int calculateAvailListLength()
{
    int length = 2 + AuthorAvailList.size()*2 - 1;

    for(int i = 0; i < AuthorAvailList.size(); i++)
    {
        length += to_string(AuthorAvailList[i].size).size() + to_string(AuthorAvailList[i].offset).size();
    }

    return length;
}

void writeAvailListTofile()
{

    fstream f;
    string fileContent = readFileToString("Authors.txt");
    fileContent = fileContent.substr(availListLength, fileContent.size() - availListLength);
    f.open("Authors.txt", ios::out);

    if(AuthorAvailList.size() == 0)
    {
        f << "[]";
        f << fileContent;
        f.close();
        return;
    }

    for(int i = 0; i < AuthorAvailList.size(); i++)
    {
        if(i == 0)
        {
            f << "[";

        }

        f << AuthorAvailList[i].offset << "," << AuthorAvailList[i].size;


        if(i != AuthorAvailList.size() -1)
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

void addToAuthorEnd(Author &a)
{
//    cout<<"inside end add";
    int recordLength = strlen(a.Author_ID) + strlen(a.Author_Name) + strlen(a.Address) + 2 + 2; // 2 for the delimiter and 2 for record length
    fstream f;
    f.open("Authors.txt", ios::in | ios::out | ios::app | ios::binary);
    int offset = readFileToString("Authors.txt").size() - availListLength;
    f.seekp(0, ios::end);
    if(recordLength < 10)
    {
        f<<0<<recordLength<<a.Author_ID<<"|"<<a.Author_Name<<"|"<<a.Address;
    }
    else
    {
        f<<recordLength<<a.Author_ID<<"|"<<a.Author_Name<<"|"<<a.Address;
    }
    AuthorPrimaryIndex[a.Author_ID] = offset;
    writePrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);
    f.close();
}

void addToAuthorOffset(Author& a, AvailListEntry& ave)
{
//    cout <<"inside offset add";
    int recordLength = strlen(a.Author_ID) + strlen(a.Author_Name) + strlen(a.Address) + 2 + 2; // 2 for the delimiter and 2 for record length
    fstream f;
    f.open("Authors.txt", ios::in | ios::out | ios::binary);
    f.seekp(ave.offset + availListLength, ios::beg);
//    cout << ave.offset << endl;
    AuthorPrimaryIndex[a.Author_ID] = ave.offset;

    ave.offset = ave.offset + recordLength;
    ave.size = ave.size - recordLength;
    if(ave.size > 0)
    {
        AuthorAvailList.push_back(ave);
    }

    if(recordLength < 10)
    {
        f<<0<<recordLength<<a.Author_ID<<"|"<<a.Author_Name<<"|"<<a.Address;
    }
    else
    {
        f<<recordLength<<a.Author_ID<<"|"<<a.Author_Name<<"|"<<a.Address;
    }
    writePrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);
    f.close();
    f.close();
}

void addAuthor(Author& a) {

    // loop through the avail list to see if there exists a deleted record where we can add
    int recordLength = strlen(a.Author_ID) + strlen(a.Author_Name) + strlen(a.Address) + 2 + 2; // 2 for the delimiter
    cout<<recordLength<<endl;
    int diff = INT_MAX;
    AvailListEntry ave;
    int index = 0;
    for (int i = 0; i < AuthorAvailList.size(); i++) {
        if(AuthorAvailList[i].size - recordLength >= 0 && AuthorAvailList[i].size - recordLength < diff)
        {
            ave = AuthorAvailList[i];
            index = i;
            diff = ave.size - recordLength;
        }
    }


    if(diff == INT_MAX)
    {
        cout << "No space left; add to the end of the file" << endl;
        addToAuthorEnd(a);
    }
    else
    {
        cout <<"Add to the empty space :"<< ave.size << " " <<ave.offset <<endl;
        AuthorAvailList.erase(AuthorAvailList.begin() + index);
        addToAuthorOffset(a, ave);
        writeAvailListTofile();
        availListLength = calculateAvailListLength();

    }
    AuthorSecondaryIndex[a.Author_Name].push_back(a.Author_ID);
    writeSecondaryIndex("AuthorSecondaryIndex.txt", AuthorSecondaryIndex);
}

void deleteAuthor(string authorID) {
    auto it = AuthorPrimaryIndex.find(authorID);

    if (it != AuthorPrimaryIndex.end()) {
        int authorOffset = it->second;

        AvailListEntry ave;
        ave.offset = authorOffset;

        fstream f;
        f.open("Authors.txt", ios::in | ios::out | ios::binary);

        f.seekg(authorOffset + availListLength, ios::beg);

        // Read the first two characters (assuming the size is stored as a two-character string)
        char authorSizeBuffer[3];  // One extra character for null terminator
        f.read(authorSizeBuffer, sizeof (authorSizeBuffer));
        authorSizeBuffer[2] = '\0';  // Null-terminate the string

        // Convert the size to an integer
        stringstream ss(authorSizeBuffer);
        int result;
        ss >> result;
        ave.size = result;

        // Add the entry to the avail list
        AuthorAvailList.push_back(ave);
        writeAvailListTofile();
        availListLength = calculateAvailListLength();

        // Move back to the position before the size to overwrite with '*'
        f.seekp(authorOffset + availListLength, ios::beg);
        f.put('*');

        f.close();

        // Remove the authorID from the secondary index
        auto secondaryIndexIt = AuthorSecondaryIndex.begin();
        while (secondaryIndexIt != AuthorSecondaryIndex.end()) {
            vector<string>& authorIDs = secondaryIndexIt->second;
            auto idIt = find(authorIDs.begin(), authorIDs.end(), authorID);
            if (idIt != authorIDs.end()) {
                authorIDs.erase(idIt);
                if (authorIDs.empty()) {
                    // If the vector is empty after removing the ID, remove the entry from the secondary index
                    secondaryIndexIt = AuthorSecondaryIndex.erase(secondaryIndexIt);
                } else {
                    ++secondaryIndexIt;
                }
            } else {
                ++secondaryIndexIt;
            }
        }

        // Update the secondary index file
        writeSecondaryIndex("AuthorSecondaryIndex.txt", AuthorSecondaryIndex);

        // Remove the entry from the primary index
        AuthorPrimaryIndex.erase(it);
        writePrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);

        cout << "Author deleted successfully" << endl;
    } else {
        cout << "Author not found" << endl;
    }
}

//void deleteAuthor(string authorID) {
//    auto it = AuthorPrimaryIndex.find(authorID);
//
//    if (it != AuthorPrimaryIndex.end()) {
//        int authorOffset = it->second;
//
//        AvailListEntry ave;
//        ave.offset = authorOffset;
//
//        fstream f;
//        f.open("Authors.txt", ios::in | ios::out | ios::binary);
//
//        f.seekg(authorOffset + availListLength, ios::beg);
//
//        // Read the first two characters (assuming the size is stored as a two-character string)
//        char authorSizeBuffer[3];  // One extra character for null terminator
//        f.read(authorSizeBuffer, sizeof (authorSizeBuffer));
//        authorSizeBuffer[2] = '\0';  // Null-terminate the string
//
//        // Convert the size to an integer
//        stringstream ss(authorSizeBuffer);
//        int result;
//        ss >> result;
//        ave.size = result;
//
//        // Add the entry to the avail list
//        AuthorAvailList.push_back(ave);
//        writeAvailListTofile();
//        availListLength = calculateAvailListLength();
//
//        // Move back to the position before the size to overwrite with '*'
//        f.seekp(authorOffset + availListLength, ios::beg);
//        f.put('*');
//
//        f.close();
//
//        // Remove the authorID from the secondary index
//        auto secondaryIndexIt = AuthorSecondaryIndex.begin();
//        while (secondaryIndexIt != AuthorSecondaryIndex.end()) {
//            vector<string>& authorIDs = secondaryIndexIt->second;
//            auto idIt = find(authorIDs.begin(), authorIDs.end(), authorID);
//            if (idIt != authorIDs.end()) {
//                authorIDs.erase(idIt);
//                if (authorIDs.empty()) {
//                    // If the vector is empty after removing the ID, remove the entry from the secondary index
//                    secondaryIndexIt = AuthorSecondaryIndex.erase(secondaryIndexIt);
//                } else {
//                    ++secondaryIndexIt;
//                }
//            } else {
//                ++secondaryIndexIt;
//            }
//        }
//
//        cout << "inside delete author" << endl;
//
//        // Update the secondary index file
//        printSecondaryIndex(AuthorSecondaryIndex);
//        writeSecondaryIndex("AuthorSecondaryIndex.txt", AuthorSecondaryIndex);
//
//        // Remove the entry from the primary index
//        AuthorPrimaryIndex.erase(it);
//        writePrimaryIndex("AuthorPrimaryIndex.txt", AuthorPrimaryIndex);
//
//        cout << "Author deleted successfully" << endl;
//    } else {
//        cout << "Author not found" << endl;
//    }
//}


void updateAuthor() {
    cout << "Enter author id:" << endl;
    string authorID;
    cin >> authorID;
    auto it = AuthorPrimaryIndex.find(authorID);

    if (it != AuthorPrimaryIndex.end()) {
        int authorOffset = it->second;
        fstream f;
        f.open("Authors.txt", ios::in | ios::binary);

        f.seekg(authorOffset + availListLength, ios::beg);
        char authorSizeBuffer[3];
        f.read(authorSizeBuffer, sizeof(authorSizeBuffer));
        authorSizeBuffer[2] = '\0';

        char *author = new char[atoi(authorSizeBuffer) + 1];
        f.seekg(authorOffset + availListLength, ios::beg);
        f.read(author, atoi(authorSizeBuffer));
        author[atoi(authorSizeBuffer)] = '\0';

        string authorAddress;
        vector<string> authorData = splitString(author, '|');
        authorAddress = authorData[2];
        cout << "Old Author Address: " << authorAddress << endl;

        delete[] author; // Free the dynamically allocated memory

        deleteAuthor(authorID);

        Author a;
        strncpy(a.Author_ID, authorID.c_str(), sizeof(a.Author_ID) - 1);
        a.Author_ID[sizeof(a.Author_ID) - 1] = '\0';  // Null-terminate the string

        cout << "Enter new author name: ";
        cin.ignore();
        cin.getline(a.Author_Name, sizeof(a.Author_Name));

        strncpy(a.Address, authorAddress.c_str(), sizeof(a.Address) - 1);
        a.Address[sizeof(a.Address) - 1] = '\0';  // Null-terminate the string

        addAuthor(a); // Add the updated author

        cout << "Author updated successfully" << endl;
    } else {
        cout << "Author not found" << endl;
    }
}

void printAuthor() {
    string authorID;
    cout << "Enter Author Id :";
    cin >> authorID;
    auto it = AuthorPrimaryIndex.find(authorID);

    if (it == AuthorPrimaryIndex.end()) {
        cout << "Author not found" << endl;
        return;
    }
    if (it != AuthorPrimaryIndex.end()) {
        int authorOffset = it->second;
        fstream f;
        f.open("Authors.txt", ios::in | ios::binary);
        f.seekg(authorOffset + availListLength, ios::beg);
        char authorSizeBuffer[3];
        f.read(authorSizeBuffer, sizeof(authorSizeBuffer));
        authorSizeBuffer[2] = '\0';

        char *author = new char[atoi(authorSizeBuffer) + 1];
        f.seekg(authorOffset + availListLength, ios::beg);

        f.read(author, atoi(authorSizeBuffer));

        author[atoi(authorSizeBuffer)] = '\0';
        vector<string> authorData = splitString(author, '|');
        string authorID = authorData[0].substr(2);  // Extract substring from the third character to the end
        string authorName = authorData[1];
        string authorAddress = authorData[2];
        cout <<  "Id :  " << authorID <<  "  Name :  " << authorName << "  Address : " << authorAddress << "" << endl;
        delete[] author; // Free the dynamically allocated memory
    }
}