#include <curses.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>

using namespace std;

struct Book{ 
    string title, author, status; 
};

struct Transaction{ 
    int id; 
    string name, date, status; 
};

map<int, Book> booksMap;
vector<Transaction> transactions;

void loadData(){
    ifstream bFile("books.txt");
    string idStr, title, author, status;
    while(getline(bFile, idStr, ',') && getline(bFile, title, ',') && getline(bFile, author, ',') && getline(bFile, status)){
        booksMap[stoi(idStr)] ={title, author, status};
    }
    bFile.close();
    ifstream iFile("issues.txt");
    string name, date, Status;
    while(getline(iFile, idStr, ',') && getline(iFile, name, ',') && getline(iFile, date, ',') && getline(iFile, Status)){
        transactions.push_back({stoi(idStr), name, date, Status});
    }
    iFile.close();
}

void saveData(){
    ofstream bFile("books.txt");
    for(auto const& x : booksMap){
        bFile << x.first << "," << x.second.title << "," << x.second.author << "," << x.second.status << endl;
    }
    bFile.close();
    ofstream iFile("issues.txt");
    for(auto const& t : transactions){
        iFile << t.id << "," << t.name << "," << t.date << "," << t.status << endl;
    }
    iFile.close();
}

int getLastId(){
    return booksMap.rbegin()->first;
}

string getInput(WINDOW* win, int y, int x, string prompt){
    echo();
    curs_set(1);
    char str[100];
    mvwprintw(win, y, x, prompt.c_str());
    wrefresh(win);
    wgetstr(win, str);
    noecho();
    curs_set(0);
    return string(str);
}

int main(){
    loadData();
    initscr();
    start_color();
    noecho();
    cbreak();
    keypad(stdscr, true);
    curs_set(0);
    init_pair(1, COLOR_WHITE, COLOR_BLACK); 
    init_pair(2, COLOR_WHITE, COLOR_CYAN);
    init_pair(3, COLOR_WHITE, COLOR_RED);     
    init_pair(4, COLOR_WHITE, COLOR_GREEN);   
    init_pair(5, COLOR_WHITE, COLOR_BLUE);  
    init_pair(6, COLOR_WHITE, COLOR_MAGENTA);  
    init_pair(7, COLOR_BLACK, COLOR_WHITE);   
    int choice = 0;
    string menu[] ={"1. Add Book", "2. Remove Book", "3. Issue Book", "4. Return Book", "5. Search Book", "6. Exit"};
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    WINDOW* menueWin = newwin(yMax - 6, xMax - 10, 4, 5); 
    while(true){
        wbkgd(stdscr, COLOR_PAIR(7));
        clear();
        attron(COLOR_PAIR(1) | A_BOLD);
        mvhline(0, 0, ' ', xMax);
        mvprintw(0,(xMax-32)/2, "GLUG's LIBRARY MANAGEMENT SYSTEM");
        attroff(COLOR_PAIR(1) | A_BOLD);
        for(int i = 0; i < 6; i++){
            if(i == choice){
                attron(A_REVERSE); 
                mvprintw(4 + i, 5, " > %s ", menu[i].c_str());
                attroff(A_REVERSE);
            } 
            else{
                mvprintw(4 + i, 5, "   %s ", menu[i].c_str());
            }
        }
        mvprintw(12, 5, "Use Arrows to Move | Enter to Select");
        refresh();
        int ch = getch();
        if(ch == KEY_UP && choice > 0){
            choice--;
        }
        else if(ch == KEY_DOWN && choice < 5){
            choice++;
        }
        else if(ch == 10){ 
            if(choice == 5){
                break;
            }
            wbkgd(stdscr, COLOR_PAIR(choice + 2));
            clear();
            attron(COLOR_PAIR(1) | A_BOLD);
            mvhline(0, 0, ' ', xMax);
            mvprintw(0,(xMax-32)/2, "GLUG's LIBRARY MANAGEMENT SYSTEM");
            attroff(COLOR_PAIR(1) | A_BOLD);
            refresh();
            wbkgd(menueWin, COLOR_PAIR(7));
            werase(menueWin);
            box(menueWin, 0, 0);
            mvwprintw(menueWin, 1, 2, "%s", menu[choice].c_str());
            wrefresh(menueWin);
            // Add
            if(choice == 0){
                string t = getInput(menueWin, 3, 2, "Enter the title of the book : ");
                string a = getInput(menueWin, 4, 2, "Enter the name of the author of the book : ");
                booksMap[getLastId() + 1] ={t, a, "Available"};
                saveData();
                mvwprintw(menueWin, 6, 2, "Book added successfully!");
            }
            // Remove
            else if(choice == 1){
                string idStr = getInput(menueWin, 3, 2, "Enter the Book ID to remove: ");
                if(!idStr.empty() && booksMap.erase(stoi(idStr))){
                    saveData();
                    mvwprintw(menueWin, 5, 2, "Book with ID has been removed.");
                } 
                else{
                    mvwprintw(menueWin, 5, 2, "Book ID not found.");
                }
            }
            // Issue
            else if(choice == 2){
                string idStr = getInput(menueWin, 3, 2, "Enter Book ID: ");
                if(!idStr.empty()){
                    int id = stoi(idStr);
                    if(booksMap.count(id) && booksMap[id].status == "Available"){
                        string name = getInput(menueWin, 4, 2, "Enter name of person issuing the book : ");
                        string date = getInput(menueWin, 5, 2, "Enter date of issuing the book : ");
                        booksMap[id].status = "Issued";
                        transactions.push_back({id, name, date, "Issued"});
                        saveData();
                        mvwprintw(menueWin, 7, 2, "Book issued successfully!");
                    } 
                    else{
                        if(!booksMap.count(id)){
                            mvwprintw(menueWin, 5, 2, "Book ID not found.");
                        }
                        else{
                            mvwprintw(menueWin, 5, 2, "Book is already issued.");
                        }
                    }
                }
            }
            // Return
            else if(choice == 3){ 
                string idStr = getInput(menueWin, 3, 2, "Enter Book ID: ");
                if(!idStr.empty()){
                    int id = stoi(idStr);
                    if(booksMap.count(id) && booksMap[id].status == "Issued"){
                        booksMap[id].status = "Available";
                        for(auto &t : transactions){
                            if(t.id == id && t.status == "Issued") t.status = "Returned";
                        }
                        saveData();
                        mvwprintw(menueWin, 5, 2, "Book returned successfully!");
                    } 
                    else{
                        if(!booksMap.count(id)){
                            mvwprintw(menueWin, 5, 2, "Book ID not found.");
                        }
                        else{
                            mvwprintw(menueWin, 5, 2, "Book was not issued.");
                        }
                    }
                }
            }
            // Search
            else if(choice == 4){
                string q = getInput(menueWin, 3, 2, "Enter Title or Author to search: ");
                int r = 5;
                for(auto const& x : booksMap){
                    if(x.second.title.find(q) != string::npos || x.second.author.find(q) != string::npos){
                        mvwprintw(menueWin, r++, 2, "ID: %d | Title: %s | Author: %s | Status: %s", x.first, x.second.title.c_str(), x.second.author.c_str(), x.second.status.c_str());
                    }
                }
                if(r == 5) mvwprintw(menueWin, 5, 2, "No books found.");
            }
            mvwprintw(menueWin, yMax - 8, 2, "Press any key to return to menue.");
            wrefresh(menueWin);
            wgetch(menueWin);
        }
    }
    endwin();
    return 0;
}