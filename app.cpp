//
// Created by LanShiyi on 6/21/15.
//

#include "app.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

// User
namespace Lib {

    class User {

    public:
        int primary_key, is_admin;
        string username, password;

        set<int> borrowed_books;

        bool login(string username, string password) {
            return username == this->username&& password == this->password;
        }

        int isAdmin() {
            return is_admin;
        }

        User(string username, string password, int primary_key, int is_admin):
                username(username),
                password(password),
                primary_key(primary_key),
                is_admin(is_admin) {}

    };

    int user_primary_key_count = 1;
    User * current_user = nullptr;

    vector<User> Users;

    // true means success
    bool login(string username, string password) {
        for (vector<User>::iterator i = Users.begin(); i != Users.end(); ++i)
            if (i->username == username) {
                current_user = &(*i);
                return i->password == password;
            }
        return false;
    }

    // always return true
    bool logout() {
        current_user = nullptr;
        return true;
    }

    // false means username is occupied
    bool register_user(string username, string password, bool auth=0) {
        for (vector<User>::iterator i = Users.begin(); i != Users.end(); ++i)
            if (i->username == username)
                return false;
        // is_admin = 0 means you can't add admin to Users
        Users.push_back(User(username, password, user_primary_key_count, auth));
        ++user_primary_key_count;
        return true;
    }


};

// Books
namespace Lib {

    class Book {

    public:
        int primary_key, rest;
        string name, description;

        Book(int primary_key, int rest, string name, string description):
                primary_key(primary_key), rest(rest), name(name), description(description)
        {}

        bool borrow() {
            if (rest > 0) {
                --rest;
                return true;
            }
            else
                return false;
        }

        // always return true
        bool return_book() {
            rest++;
            return true;
        }
    };

    vector<Book> Books;
    int book_primary_key_count = 0;

    bool borrow(int primary_key) {

        Book * it;

        // out of range
        bool found = 0;
        for (vector<Book>::iterator i = Books.begin(); i != Books.end(); ++i)
            if (i->primary_key == primary_key) {
                found = 1;
                it = &(*i);
            }
        if (!found)
            return false;

        // not log in
        if (current_user == nullptr)
            return false;

        // You have borrowed it
        if (current_user->borrowed_books.find(primary_key) != current_user->borrowed_books.end())
            return false;

        // lent out
        if (!(it->borrow()))
            return false;

        //success
        current_user->borrowed_books.insert(primary_key);

        return true;
    }

    bool return_book(int primary_key) {


        // out of range
        bool found = 0;
        vector<Book>::iterator it;
        for (vector<Book>::iterator i = Books.begin(); i != Books.end(); ++i)
            if (i->primary_key == primary_key) {
                it = i;
                found = 1;
            }
        if (!found)
            return false;

        // not log in
        if (current_user == nullptr)
            return false;

        // You didn't borrow it
        if (current_user->borrowed_books.find(primary_key) == current_user->borrowed_books.end())
            return false;

        //success
        it->return_book();
        current_user->borrowed_books.erase(primary_key);

        return true;
    }

    bool add_book() {

        string name, description;
        int rest;

        cerr << "Enter book name" << endl;
        cin >> name;

        cerr << "Enter book description" << endl;
        cin >> description;

        cerr << "How many do you have?" << endl;
        cin >> rest;

        Books.push_back(Book(book_primary_key_count, rest, name, description));
        ++book_primary_key_count;

        return true;
    }

    bool delete_book(int primary_key) {
        // out of range
        bool found = 0;
        vector<Book>::iterator it;
        for (vector<Book>::iterator i = Books.begin(); i != Books.end(); ++i)
            if (i->primary_key == primary_key) {
                it = i;
                found = 1;
            }
        if (!found)
            return false;

        Books.erase(it);
        return true;
    }
};

int main() {

    Lib::register_user("admin", "admin", 1);

    while (1) {

        string opt;

        cerr << "Now you can:" << endl;
        if (Lib::current_user == nullptr) {

            // list operation
            cerr << "login" << endl;
            cerr << "register" << endl;

            cin >> opt;

            if (opt == "login") {

                string username, password;

                cerr << "username: " << endl;
                cin >> username;
                cerr << "password: " << endl;
                cin >> password;

                cerr << (Lib::login(username, password) ? "login successfully" : "wrong username or password") << endl;
            }
            else {

                string username, password;

                cerr << "username: " << endl;
                cin >> username;
                cerr << "password: " << endl;
                cin >> password;

                cerr << (Lib::register_user(username, password) ? "register successfully" : "username is occupied") << endl;
            }

        }
        else {

            cerr << "list" << endl;
            cerr << "borrow(input primary_key)" << endl;
            cerr << "return(input primary_key)" << endl;
            cerr << "logout" << endl;
            if (Lib::current_user->isAdmin())
                cerr << "add" << endl;

            cin >> opt;

            if (Lib::current_user != nullptr) {
                if (opt == "list") {
                    for (vector<Lib::Book>::iterator i = Lib::Books.begin(); i != Lib::Books.end(); ++i) {
                        cerr << "Book name:" << i->name << endl
                            << "        " << i->description << endl
                            << "        " << "rest: " << i->rest << endl
                            << "        " << "primary_key: " << i->primary_key << endl;
                    }
                }
                else if (opt == "borrow") {
                    int pk;
                    cin >> pk;
                    cerr << (Lib::borrow(pk) ? "success": "error") << endl;
                }
                else if (opt == "return") {
                    int pk;
                    cin >> pk;
                    cerr << (Lib::return_book(pk) ? "success": "error") << endl;
                }
                else if (opt == "logout") {
                    Lib::logout();
                }

                if (opt == "add") {
                    if (Lib::current_user->isAdmin()){
                        Lib::add_book();
                    }
                    else {
                        cerr << "You are not admin" << endl;
                    }
                }
            }
        }
        cerr << endl << endl << endl;
    }

    return 0;
}
