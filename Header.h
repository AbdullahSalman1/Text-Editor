#include <iostream>
#include <Windows.h>
using namespace std;

void gotoxy(int x, int y);
void printBoundaries();

struct Node {
    char data;
    Node* left;
    Node* right;
    Node* up;
    Node* down;

    Node(char c = '\0') : data(c), left(nullptr), right(nullptr), up(nullptr), down(nullptr) {}
};

class Notepad {
public:
    Node* head;
    Node* currLineHead;
    Node* prevLineHead;
    Node* prevCurr;
    Node* cursor;

    Notepad() {
        head = new Node(' ');
        currLineHead = head;
        prevLineHead = nullptr;
        prevCurr = nullptr;
        cursor = head;
    }

    void insertChar(char c) {
        static Node* wordStart = nullptr;

        // handle Enter key or character limit
        if (c == 13 || getCursorXOffset() >= 90) {
            if (wordStart != nullptr && getCursorXOffset() >= 90) {
                moveWordToNewLine(wordStart);
                return;
            }

            Node* newLine = new Node(' ');

            // check if there's already a line below with text
            if (currLineHead->down != nullptr) {
                Node* tempLine = currLineHead->down;
                currLineHead->down = newLine;
                newLine->up = currLineHead;
                newLine->down = tempLine;
                tempLine->up = newLine;
            }
            else {
                currLineHead->down = newLine;
                newLine->up = currLineHead;
            }

            // move all characters after the cursor to the new line
            Node* temp = cursor->right;
            cursor->right = nullptr;

            if (temp != nullptr) {
                newLine->right = temp;
                temp->left = newLine;

                Node* moveTemp = newLine->right;
                while (moveTemp != nullptr) {
                    if (moveTemp->up != nullptr && moveTemp->up->right == moveTemp) {
                        moveTemp->up->right = nullptr; // disconnect from upper line
                    }
                    moveTemp = moveTemp->right;
                }
            }

            cursor = newLine;
            prevLineHead = currLineHead;
            currLineHead = newLine;

            prevCurr = prevLineHead;

            system("cls");
            printText(1, 1);

            return;
        }

        // Regular insertion logic for characters
        Node* newNode = new Node(c);

        // Track word start if it's the beginning of a new word
        if (wordStart == nullptr && c != ' ') {
            wordStart = cursor->right == nullptr ? newNode : cursor->right;
        }

        if (cursor->right == nullptr) {
            cursor->right = newNode;
            newNode->left = cursor;

            // Handle linking with the previous line's nodes
            if (prevCurr != nullptr && prevCurr->right != nullptr) {
                prevCurr = prevCurr->right;
                prevCurr->down = newNode;
                newNode->up = prevCurr;
            }
        }
        else {
            newNode->right = cursor->right;
            cursor->right->left = newNode;
            newNode->left = cursor;
            cursor->right = newNode;

            // Adjust up-down pointers
            if (cursor->up != nullptr && cursor->up->right != nullptr) {
                newNode->up = cursor->up->right;
                cursor->up->right->down = newNode;
            }

            if (cursor->down != nullptr && cursor->down->right != nullptr) {
                newNode->down = cursor->down->right;
                cursor->down->right->up = newNode;
            }
        }

        if (c == ' ') {
            wordStart = nullptr;
        }

        cursor = newNode;
        system("cls");
        printText(1, 1);
    }

    void moveWordToNewLine(Node* start) {
        if (start == nullptr) return;

        Node* newLine = new Node(' ');

        // Handle linking the new line
        if (currLineHead->down != nullptr) {
            Node* tempLine = currLineHead->down;
            currLineHead->down = newLine;
            newLine->up = currLineHead;
            newLine->down = tempLine;
            tempLine->up = newLine;
        }
        else {
            currLineHead->down = newLine;
            newLine->up = currLineHead;
        }

        // Move entire word to the new line
        Node* temp = start;
        Node* prev = nullptr;

        // Track the last node of the word
        Node* lastMovedNode = nullptr;

        while (temp != nullptr) {
            Node* next = temp->right;

            // Remove the word from the upper line
            if (temp->left != nullptr) {
                temp->left->right = temp->right;
            }
            else {
                currLineHead->right = temp->right;  // Adjust line head
            }
            if (temp->right != nullptr) {
                temp->right->left = temp->left;
            }

            temp->left = prev;
            if (prev != nullptr) {
                prev->right = temp;
            }
            else {
                newLine->right = temp;
            }
            temp->up = nullptr;
            temp->down = nullptr;

            prev = temp;
            lastMovedNode = temp;
            temp = next;
        }

        cursor = lastMovedNode;

        if (cursor != nullptr) {
            cursor->right = nullptr;
        }

        prevLineHead = currLineHead;
        currLineHead = newLine;

        system("cls");
        printText(1, 1);
    }

    void backspace() {
        if (cursor->left != nullptr) {
            Node* toDelete = cursor;
            cursor = cursor->left;

            cursor->right = toDelete->right;
            if (toDelete->right != nullptr) {
                toDelete->right->left = cursor;
            }

            delete toDelete;

            system("cls");
            printText(1, 1);
        }

        else if (cursor->left == nullptr && prevLineHead != nullptr) {
            Node* endNode = prevLineHead;
            while (endNode->right != nullptr) {
                endNode = endNode->right;
            }

            currLineHead = prevLineHead;

            if (currLineHead->up != nullptr) {
                prevLineHead = currLineHead->up;
            }

            cursor = endNode;

            system("cls");
            printText(1, 1);
        }
    }

    // Move cursor functions
    void moveLeft() {
        if (cursor->left != nullptr)
            cursor = cursor->left;
    }

    void moveRight() {
        if (cursor->right != nullptr)
            cursor = cursor->right;
    }

    void moveUp() {
        if (currLineHead->up != nullptr) {
            int currentXOffset = getCursorXOffset();

            int upperXOffset = 0;
            Node* target = currLineHead->up;

            Node* temp = target;
            while (temp->right != nullptr) {
                temp = temp->right;
                upperXOffset++;
            }

            if (upperXOffset < currentXOffset) {
                cursor = temp;
            }
            else {
                target = currLineHead->up;
                for (int i = 0; i < currentXOffset && target->right != nullptr; i++) {
                    target = target->right;
                }
                cursor = target;
            }

            prevLineHead = (currLineHead->up != nullptr) ? currLineHead->up : nullptr;
            currLineHead = currLineHead->up;
        }

        gotoxy(1 + getCursorXOffset(), getCursorYOffset() - 1);
    }


    void moveDown() {
        if (currLineHead->down != nullptr) {
            // Get the X-offset of the current cursor position
            int currentXOffset = getCursorXOffset();

            // Calculate the X-offset for the lower line
            int lowerXOffset = 0;
            Node* target = currLineHead->down;

            // Traverse to the end of the lower line to calculate the X-offset
            Node* temp = target;
            while (temp->right != nullptr) {
                temp = temp->right;
                lowerXOffset++;
            }

            // Compare the X-offsets
            if (lowerXOffset < currentXOffset) {
                cursor = temp;
            }
            else {
                // Move cursor to the same X-offset position in the lower line
                target = currLineHead->down;
                for (int i = 0; i < currentXOffset && target->right != nullptr; i++) {
                    target = target->right;
                }
                cursor = target;
            }

            prevLineHead = currLineHead;
            currLineHead = currLineHead->down;
        }

        gotoxy(1 + getCursorXOffset(), getCursorYOffset() + 1);
    }


    void printText(int x, int y) {
        Node* row = head;
        int currentY = y;

        while (row != nullptr) {
            gotoxy(x, currentY);
            Node* current = row;

            while (current != nullptr) {
                cout << current->data;
                current = current->right;
            }

            currentY++;  // Move to the next line
            row = row->down;
        }

        // Print boundaries after printing text
        printBoundaries();
        // Move the cursor to the current position after text insertion
        gotoxy(x + getCursorXOffset() + 1, y + getCursorYOffset());
    }


    int getCursorXOffset() {
        int offset = 0;
        Node* current = cursor;

        while (current->left != nullptr) {
            current = current->left;
        }

        Node* start = current;
        while (start != cursor) {
            start = start->right;
            offset++;
        }

        return offset;
    }

    int getCursorYOffset() {
        int offset = 0;
        Node* current = currLineHead;

        while (current->up != nullptr) {
            current = current->up;
            offset++;
        }

        return offset;
    }

    friend void gotoxy(int x, int y);
    friend void printBoundaries();
};

void printBoundaries() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    int textAreaWidth = width * 0.8;  // 80% width (excluding search area)
    int textAreaHeight = height * 0.8; // 80% height (excluding suggestions area)

    for (int i = 0; i <= width; i++) {
        gotoxy(i, textAreaHeight + 1);
        cout << "-";
    }
    gotoxy(0, textAreaHeight + 2);
    cout << "Word Suggestions";

    // Draw the right boundary
    bool flag = true;
    for (int i = 0; i <= height; i++) {
        gotoxy(textAreaWidth + 1, i);
        cout << "|";
        if (flag) {
            cout << "Search";
            flag = false;
        }
    }
}

void gotoxy(int x, int y) {
    COORD c = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

int main(int argc, char* argv[]) {
    system("cls"); // Clear the screen

    HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE); // handle to read console

    DWORD Events = 0;     // Event count
    DWORD EventsRead = 0; // Events read from console

    Notepad notepad;

    bool Running = true;

    int x = 1, y = 1; // Initial cursor position

    // Print initial boundaries
    printBoundaries();

    gotoxy(x, y);

    // Program's main loop
    while (Running) {
        // Get the system's current "event" count
        GetNumberOfConsoleInputEvents(rhnd, &Events);

        if (Events != 0) { // if something happened, handle the events we want
            INPUT_RECORD eventBuffer[128];

            // fills the event buffer with the events and saves count in EventsRead
            ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);

            // loop through the event buffer using the saved count
            for (DWORD i = 0; i < EventsRead; ++i) {

                // check if event[i] is a key event && if so is a press not a release
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {
                    char key = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;
                    // check if the key press was an arrow key
                    switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode) {

                    case VK_LEFT:
                        notepad.moveLeft();
                        break;
                    case VK_RIGHT:
                        notepad.moveRight();
                        break;
                    case VK_UP:
                        notepad.moveUp();
                        break;
                    case VK_DOWN:
                        notepad.moveDown();
                        break;
                    case VK_BACK:
                        notepad.backspace();
                        break;
                    default:
                        notepad.insertChar(key); // Insert regular characters
                        break;
                    }

                    notepad.printText(x, y);
                    gotoxy(x + notepad.getCursorXOffset() + 1, y + notepad.getCursorYOffset());

                }
            }
        }
    }

    return 0;
}
