//<----------Assignment#2 updated------------->
#include <iostream>
#include <Windows.h>
#include <fstream> 

#include <stack> 
using namespace std;


stack<int>xCoordinate;
stack<int>yCoordinate;
stack<char>redo;

void virtualdisplay(int x, int y);
void PrintBorders();


struct Pointers {
	char value;
	Pointers* Left;
	Pointers* Right;
	Pointers* Up;
	Pointers* Down;

	Pointers(char c = '\0') {
		value = c;
		Left = NULL;
		Right = NULL;
		Up = NULL;
		Down = NULL;
	}
};
stack<int> redox;

stack<int>redoy;
#include <string> 
stack<int>wordsize;






class TextEditor {
public:
	Pointers* headPointer;
	Pointers* blinkerheadpointer;
	Pointers* previousPointer;
	Pointers* previousrecordPointer;
	Pointers* blinker;
	int currentRow;
	int stackcounter;
	TextEditor() {

		previousPointer = NULL;
		headPointer = new Pointers(' ');
		blinker = headPointer;
		previousrecordPointer = NULL;
		blinkerheadpointer = headPointer;
		stackcounter = 0;
		currentRow = 0;
	}


	void insertChar(char c) {
		static Pointers* StartingOfNewWord = NULL;
		if (c == 13 && Ycursorposition() < 23) {
			NextLineFunction(StartingOfNewWord);
			return;
		}

		if (c == 26) {
			UndoFunction();
			return;
		}
		if (c == 24) {
			RedoFunction();
			return;
		}
		if ((c >= 'A' && c <= 'Z') || c == ' ' || (c >= 'a' && c <= 'z')) {
			int x = 0;
			Pointers* positionofpointer = blinker;
			positionofpointer = moveToStart(positionofpointer);

			while (positionofpointer != blinker) {
				positionofpointer = positionofpointer->Right;
				x++;
			}
			if (x >= 70 && Ycursorposition() < 23) {
				NextLineFunction(StartingOfNewWord);
				return;
			}
			Pointers* newNode = new Pointers(c);
			updateWordStart(StartingOfNewWord, newNode, c);

			if (blinker->Right == NULL) {
				insertAtEnd(newNode);
			}
			else {
				insertInMiddle(newNode);
			}

			if (c == ' ') {
				StartingOfNewWord = NULL;
			}

			if (c == ' ' && (blinker->Left->value >= 'A' || blinker->Left->value <= 'Z' || blinker->Left->value >= 'a' || blinker->Left->value <= 'z')) {
				xCoordinate.push(Xcursorposition());
				yCoordinate.push(Ycursorposition());
			}

			blinker = newNode;
			system("cls");
			display();
		}
		else {
			return;
		}
	}


	//Undo function
	void UndoFunction() {
		if (stackcounter > 4) {
			return;
		}
		if (xCoordinate.empty() || yCoordinate.empty()) {
			return;
		}
		int topx = xCoordinate.top();
		int topy = yCoordinate.top();
		xCoordinate.pop();
		yCoordinate.pop();

		virtualdisplay(topx, topy);
		Pointers* tempPointer = headPointer;
		for (int y = 0; y < topy; y++) {
			if (tempPointer->Down != NULL) {
				tempPointer = tempPointer->Down;
			}
			else {
				break;
			}
		}
		blinker = tempPointer;
		for (int x = 0; x < topx; x++) {
			if (blinker->Right != NULL) {
				blinker = blinker->Right;
			}
			else {
				break;
			}
		}

		int size = 0;
		Pointers* temp = blinker;
		while (temp != NULL && temp->value != ' ') {
			redo.push(temp->value);
			temp = temp->Left;
			size++;
		}
		redox.push(topx - size);
		redoy.push(topy);
		wordsize.push(size);
		for (int i = 0; i < size; i++) {
			backspace();
		}
		while (temp->Left != NULL && temp->value != ' ') {
			temp = temp->Left;
		}
		stackcounter++;
	}

	//Redo function
	void RedoFunction() {
		if (redox.empty() || redoy.empty()) {
			return;
		}

		int topx = redox.top();
		int topy = redoy.top();
		redox.pop();
		redoy.pop();

		virtualdisplay(topx, topy);


		Pointers* tempPointer = headPointer;
		for (int y = 0; y < topy; y++) {
			if (tempPointer->Down != NULL) {
				tempPointer = tempPointer->Down;
			}
			else {
				break;
			}
		}
		blinker = tempPointer;

		// Traverse to the appropriate column
		for (int x = 0; x < topx; x++) {
			if (blinker->Right != NULL) {
				blinker = blinker->Right;
			}
			else {
				break;
			}
		}
		int n = wordsize.top();
		wordsize.pop();
		int i = 0;
		while (i < n) {
			insertChar(redo.top());
			redo.pop();
			i++;
		}
		stackcounter--;
	}




	//moving text to next line
	void  NextLineFunction(Pointers*& Starting) {
		if (Starting != NULL) {
			if (Xcursorposition() >= 70) {
				wordonnextline(Starting);
				return;
			}
		}
		Pointers* NextLine = new Pointers(' ');
		bool hasNextLine = (blinkerheadpointer->Down != nullptr);
		Pointers* temp = hasNextLine ? blinkerheadpointer->Down : nullptr;

		blinkerheadpointer->Down = NextLine;
		NextLine->Up = blinkerheadpointer;
		NextLine->Down = hasNextLine ? temp : nullptr;
		if (hasNextLine) temp->Up = NextLine;

		moveToNewLine(NextLine);
	}

	// Function to move text to the new line
	void moveToNewLine(Pointers* Nextline) {
		Pointers* xyz = blinker->Right;
		blinker->Right = NULL;

		if (xyz != nullptr) {
			Nextline->Right = xyz;
			xyz->Left = Nextline;
			Pointers* xyz = Nextline->Right;
			for (;xyz != NULL;) {
				if (xyz->Up != nullptr) {
					if (xyz->Up->Right == xyz) {
						xyz->Up->Right = NULL;
					}
				}
				xyz = xyz->Right;
			}
		}
		blinker = Nextline;
		previousPointer = blinkerheadpointer;
		blinkerheadpointer = Nextline;
		previousrecordPointer = previousPointer;

		system("cls");
		display();
	}

	// Function to update the word start for new words
	void updateWordStart(Pointers*& Starting, Pointers* newNode, char c) {
		Starting = (Starting == nullptr && c != ' ') ? (blinker->Right == nullptr ? newNode : blinker->Right) : Starting;
	}

	// Function to insert a node at the end of the line
	void insertAtEnd(Pointers* AddNode) {
		blinker->Right = AddNode;
		AddNode->Left = blinker;

		//linking with prev lines
		bool hasPrevRight = (previousrecordPointer != nullptr && previousrecordPointer->Right != nullptr);
		previousrecordPointer = hasPrevRight ? previousrecordPointer->Right : previousrecordPointer;

		if (hasPrevRight) {
			previousrecordPointer->Down = AddNode;
			AddNode->Up = previousrecordPointer;
		}

	}



	// Function to insert a node in the middle of the line
	void insertInMiddle(Pointers* AddNode) {

		SettingLeftRightPointers(AddNode);
	}

	void SettingLeftRightPointers(Pointers* AddNode) {
		bool hasRight = (blinker->Right != nullptr);

		AddNode->Right = hasRight ? blinker->Right : nullptr;
		if (hasRight) {
			blinker->Right->Left = AddNode;
		}

		AddNode->Left = blinker;
		blinker->Right = AddNode;

		UpDownPointers(AddNode);
	}


	void UpDownPointers(Pointers* AddNode) {
		// Adjust up-down pointers
		if (blinker->Up != NULL) {
			if (blinker->Up->Right != NULL) {
				AddNode->Up = blinker->Up->Right;
				blinker->Up->Right->Down = AddNode;
			}
		}
		if (blinker->Down != NULL) {
			if (blinker->Down->Right != NULL) {
				AddNode->Down = blinker->Down->Right;
				blinker->Down->Right->Up = AddNode;
			}
		}
	}


	//Divide it into functions
	void wordonnextline(Pointers* wordStartPointer) {
		if (wordStartPointer == NULL)
			return;

		Pointers* newLinePointer = new Pointers(' ');

		// Handle linking the new line using a ternary operator
		Pointers* tempNextLinePointer = (blinkerheadpointer->Down != nullptr)
			? blinkerheadpointer->Down
			: nullptr;

		blinkerheadpointer->Down = newLinePointer;
		newLinePointer->Up = blinkerheadpointer;

		newLinePointer->Down = (tempNextLinePointer != nullptr)
			? tempNextLinePointer
			: nullptr;

		if (tempNextLinePointer != nullptr) {
			tempNextLinePointer->Up = newLinePointer;
		}

		// Move entire word to the new line
		Pointers* currentPointer = wordStartPointer;
		Pointers* previousPointer = NULL;

		// Track the last node of the word
		Pointers* lastMovedPointer = NULL;

		while (currentPointer != nullptr) {

			for (;currentPointer != NULL;) {
				Pointers* nextPointer = currentPointer->Right;

				// Use ternary operators to handle removal of word from the upper line
				if (currentPointer->Left != NULL) {
					currentPointer->Left->Right = currentPointer->Right;
				}
				else {
					blinkerheadpointer->Right = currentPointer->Right;
				}

				if (currentPointer->Right != NULL) {
					currentPointer->Right->Left = currentPointer->Left;
				}

				currentPointer->Left = previousPointer;
				if (previousPointer != NULL) {
					previousPointer->Right = currentPointer;
				}
				else {
					newLinePointer->Right = currentPointer;
				}

				currentPointer->Up = NULL;
				currentPointer->Down = NULL;

				previousPointer = currentPointer;
				lastMovedPointer = currentPointer;
				currentPointer = nextPointer;
			}

			blinker = lastMovedPointer;

			if (blinker != NULL) {
				blinker->Right = NULL;
			}

			previousPointer = blinkerheadpointer;
			blinkerheadpointer = newLinePointer;

			system("cls");
			display();
		}
	}
	// Function to delete the character at the cursor position
	void deleteCurrentCharacter() {
		Pointers* charToDelete = blinker;
		blinker = blinker->Left;

		// Relink pointers to bypass the deleted node
		blinker->Right = charToDelete->Right;
		if (charToDelete->Right != NULL) {
			charToDelete->Right->Left = blinker;
		}

		delete charToDelete;

		system("cls");
		display();
	}

	// Function to merge the current line with the previous line
	void mergeWithPreviousLine() {
		Pointers* lineEndPointer = previousPointer;


		while (lineEndPointer->Right != NULL) {
			lineEndPointer = lineEndPointer->Right;
		}

		blinkerheadpointer = previousPointer;

		// Update the previous line head pointer if there's an upper line
		previousPointer = (blinkerheadpointer->Up != NULL) ? blinkerheadpointer->Up : previousPointer;

		blinker = lineEndPointer;

		system("cls");
		display();
	}


	void backspace() {
		;
		if (blinker->Left != nullptr) {
			deleteCurrentCharacter();
		}

		else if (blinker->Left == nullptr && previousPointer != nullptr) {
			mergeWithPreviousLine();
		}
	}



	void moveCursor(char ch) {
		int x = 0;
		Pointers* positionofpointer = blinker;
		positionofpointer = moveToStart(positionofpointer);

		while (positionofpointer != blinker) {
			positionofpointer = positionofpointer->Right;
			x++;
		}
		x += 1;
		int y = Ycursorposition() + 1;
		switch (ch) {
		case 'L':
			if (blinker->Left != NULL)
				blinker = blinker->Left;
			break;
		case 'R':
			if (blinker->Right != NULL)
				blinker = blinker->Right;
			break;
		case 'U':
			if (blinkerheadpointer->Up != nullptr) {
				int currentXposition = Xcursorposition();

				int upperXOffset = 0;
				Pointers* destination = blinkerheadpointer->Up;

				Pointers* temp = destination;
				while (temp->Right != nullptr) {
					temp = temp->Right;
					upperXOffset++;
				}

				if (upperXOffset < currentXposition) {
					blinker = temp;
				}
				else {
					destination = blinkerheadpointer->Up;
					int i = 0;
					while (i < currentXposition && destination->Right != NULL) {
						destination = destination->Right;
						i++;
					}

					blinker = destination;
				}


				if (blinkerheadpointer->Up != NULL) {
					previousPointer = blinkerheadpointer->Up;
				}
				else {
					previousPointer = NULL;
				}

				blinkerheadpointer = blinkerheadpointer->Up;
			}

			virtualdisplay(x, y);
			break;
		case 'D':
			if (blinkerheadpointer->Down != NULL) {

				int currentXOffset = Xcursorposition();


				int lowerXOffset = 0;
				Pointers* target = blinkerheadpointer->Down;


				Pointers* temp = target;
				while (temp->Right != NULL) {
					temp = temp->Right;
					lowerXOffset++;
				}


				if (lowerXOffset < currentXOffset) {
					blinker = temp;
				}
				else {

					target = blinkerheadpointer->Down;
					int i = 0;
					while (i < currentXOffset && target->Right != NULL) {
						target = target->Right;
						i++;
					}

					blinker = target;
				}

				previousPointer = blinkerheadpointer;
				blinkerheadpointer = blinkerheadpointer->Down;
			}

			virtualdisplay(x, y);
			break;


		}

	}



	void display() {
		Pointers* currentRowPointer = headPointer;
		int currentRowPosition = 1;

		while (currentRowPointer != NULL) {

			virtualdisplay(1, currentRowPosition);

			Pointers* currentCharPointer = currentRowPointer;


			while (currentCharPointer != NULL) {
				cout << currentCharPointer->value;
				currentCharPointer = currentCharPointer->Right;
			}

			currentRowPosition++;
			currentRowPointer = currentRowPointer->Down;
		}


		PrintBorders();


		int x = 2 + Xcursorposition();
		int y = 1 + Ycursorposition();
		virtualdisplay(x, y);
	}



	int Xcursorposition() {
		int offset = 0;
		Pointers* current = blinker;
		current = moveToStart(current);

		while (current != blinker) {
			current = current->Right;
			offset++;
		}

		return offset;
	}

	// Function to move to the start of the line
	Pointers* moveToStart(Pointers* current) {
		while (current->Left != NULL) {
			current = current->Left;
		}
		return current;
	}

	int Ycursorposition() {
		int offset = 0;
		Pointers* current = blinkerheadpointer;

		// Count the number of lines above the current line
		while (current->Up != NULL) {
			current = current->Up;
			offset++;
		}

		return offset;
	}


	friend void gotoxy(int x, int y);
	friend void printBoundaries();

	void saveToFile(const string& filename) {
		ofstream outFile(filename);
		if (outFile.is_open()) {
			Pointers* currentLine = headPointer;
			while (currentLine != NULL) {
				Pointers* currentChar = currentLine;
				while (currentChar != NULL) {
					outFile << currentChar->value;
					currentChar = currentChar->Right;
				}
				outFile << '\n';
				currentLine = currentLine->Down;
			}
			outFile.close();
		}
		else {
			cout << "Unable to open file." << endl;
		}
	}


	// Function to load text from a file
	void loadFromFile(const string& filename) {
		ifstream inFile(filename);
		if (inFile.is_open()) {
			clearText();
			string line;
			while (getline(inFile, line)) {
				for (char c : line) {
					insertChar(c);
				}
				insertChar('\n');
			}
			inFile.close();
		}
		else {
			cout << "Unable to open file for reading." << endl;
		}
	}

	// Function to clear the text in the editor (for loading new text)
	void clearText() {
		while (blinkerheadpointer->Down != NULL) {
			Pointers* temp = blinkerheadpointer->Down;
			delete blinkerheadpointer;
			blinkerheadpointer = temp;
		}

		// Clear the current line content
		while (headPointer->Right != NULL) {
			Pointers* temp = headPointer->Right;
			delete headPointer;
			headPointer = temp;
		}
	}

};




void DrawHorizontalLine(int length, int y) {
	virtualdisplay(0, y);
	for (int i = 0; i < length; i++) {
		cout << "-";
	}
}

void DrawVerticalLine(int height, int x) {
	for (int i = 0; i < height; i++) {
		virtualdisplay(x, i);
		cout << "|";
	}
}

void PrintBorders() {
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);

	// Calculate dimensions
	int totalWidth = (consoleInfo.srWindow.Right - consoleInfo.srWindow.Left) + 1;
	int totalHeight = (consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top) + 1;
	int textAreaWidth = (totalWidth * 3) / 5; // 60% width for text area
	int textAreaHeight = (totalHeight * 4) / 5; // 80% height for text area


	DrawHorizontalLine(textAreaWidth, 0);


	DrawVerticalLine(textAreaHeight, 0);
	DrawVerticalLine(textAreaHeight, textAreaWidth);


	DrawHorizontalLine(textAreaWidth, textAreaHeight + 1);

	virtualdisplay(0, textAreaHeight + 2);
	cout << "Word Suggestions";
	virtualdisplay(textAreaWidth + 1, 0);
	cout << "Search";
}



void virtualdisplay(int x, int y) {
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}





int main(int argc, char* argv[]) {
	system("cls");
	DWORD Events = 0;
	DWORD EventsRead = 0;
	HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);



	TextEditor texteditor;

	bool Running = true;
	// Load text from a file
	cout << "1 : Create a new file " << endl;
	cout << "2 : Go with existing file " << endl;
	cout << "3 : Exit " << endl;
	string s;
	int n;
	cin >> n;
	if (n == 1) {
		cout << "Enter the name of new file " << endl;
		cin >> s;
	}
	else if (n == 2) {
		cout << "Enter the file name you want to work with " << endl;
		cin >> s;
	}
	else if (n == 3) {
		return 0;
	}
	s += ".txt";

	texteditor.loadFromFile(s);
	system("cls");


	// Print initial boundaries
	PrintBorders();

	virtualdisplay(1, 1);

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

					// Handle key events using if statements
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT) {
						texteditor.moveCursor('L');
					}

					else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT) {
						texteditor.moveCursor('R');
					}
					else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_UP) {
						texteditor.moveCursor('U');
					}
					else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN) {
						texteditor.moveCursor('D');


					}
					else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_BACK) {
						texteditor.backspace();
					}
					else {
						texteditor.insertChar(key);
					}

					texteditor.display();
					virtualdisplay(texteditor.Xcursorposition() + 2, 1 + texteditor.Ycursorposition());
				}
			}
		}



		texteditor.saveToFile(s);
	}


	return 0;
}