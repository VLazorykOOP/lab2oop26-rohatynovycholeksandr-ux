#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <fstream>
#include <windows.h>

using namespace std;

//////////////////////////////////////////////////////////////
// Enable UTF-8 (for correct text output)
//////////////////////////////////////////////////////////////
void initConsole()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

//////////////////////////////////////////////////////////////
// Structure with bit fields (16 bits total)
//////////////////////////////////////////////////////////////
struct BitFields
{
    unsigned row : 4;   // row number (0-15)
    unsigned high : 4;  // high 4 bits of ASCII
    unsigned p1 : 1;    // parity bit 1
    unsigned low : 4;   // low 4 bits of ASCII
    unsigned pos : 2;   // position of character
    unsigned p2 : 1;    // parity bit 2
};

//////////////////////////////////////////////////////////////
// Union to access data as number and as bits
//////////////////////////////////////////////////////////////
union DataWord
{
    BitFields bits;
    unsigned short value;
};

//////////////////////////////////////////////////////////////
// Parity function (XOR)
//////////////////////////////////////////////////////////////
int parity(unsigned short value, int start, int end)
{
    int p = 0;

    for (int i = start; i <= end; i++)
    {
        p ^= (value >> i) & 1;
    }

    return p;
}

//////////////////////////////////////////////////////////////
// 3.1 Input text and save to TEXT file
//////////////////////////////////////////////////////////////
vector<string> inputText()
{
    vector<string> text(16);

    cout << "Enter 16 lines (max 4 characters):\n";

    for (int i = 0; i < 16; i++)
    {
        cout << "Line " << i << ": ";
        cin >> text[i];

        if (text[i].length() > 4)
            text[i] = text[i].substr(0, 4);
    }

    // save to file
    ofstream file("text.txt");
    for (int i = 0; i < 16; i++)
    {
        file << text[i] << endl;
    }
    file.close();

    cout << "Text saved to file\n";

    return text;
}

//////////////////////////////////////////////////////////////
// 3.2 Encoding + binary file (using bit fields + union)
//////////////////////////////////////////////////////////////
vector<unsigned short> encode(vector<string> text)
{
    vector<unsigned short> data;

    ofstream file("data.bin", ios::binary);

    for (int row = 0; row < 16; row++)
    {
        for (int pos = 0; pos < 4; pos++)
        {
            DataWord word;

            char ch = text[row][pos];
            int ascii = (int)ch;

            int high = (ascii >> 4) & 0xF;
            int low = ascii & 0xF;

            // fill structure
            word.bits.row = row;
            word.bits.high = high;
            word.bits.low = low;
            word.bits.pos = pos;

            // calculate parity bits
            word.bits.p1 = parity(word.value, 0, 7);
            word.bits.p2 = parity(word.value, 9, 14);

            // save to vector
            data.push_back(word.value);

            // write to binary file
            file.write((char*)&word.value, sizeof(word.value));

            // print result
            cout << "Row " << row
                << " Pos " << pos
                << " -> " << bitset<16>(word.value)
                << endl;
        }
    }

    file.close();

    cout << "Encoding finished\n";

    return data;
}

//////////////////////////////////////////////////////////////
// 3.4 Read binary file into vector
//////////////////////////////////////////////////////////////
vector<unsigned short> readBinary()
{
    vector<unsigned short> data;

    ifstream file("data.bin", ios::binary);

    unsigned short word;

    while (file.read((char*)&word, sizeof(word)))
    {
        data.push_back(word);
    }

    file.close();

    cout << "\nData from file:\n";

    for (int i = 0; i < data.size(); i++)
    {
        cout << bitset<16>(data[i]) << endl;
    }

    return data;
}

//////////////////////////////////////////////////////////////
// 3.3 CRC check
//////////////////////////////////////////////////////////////
void checkCRC(vector<unsigned short> data)
{
    cout << "\nChecking CRC...\n";

    for (int i = 0; i < data.size(); i++)
    {
        unsigned short word = data[i];

        int odd = 0;
        int even = 0;

        for (int j = 0; j < 14; j++)
        {
            if (j % 2 == 0)
                even ^= (word >> j) & 1;
            else
                odd ^= (word >> j) & 1;
        }

        int bit14 = (word >> 14) & 1;
        int bit15 = (word >> 15) & 1;

        cout << bitset<16>(word) << " -> ";

        if (bit14 == odd && bit15 == even)
            cout << "OK\n";
        else
            cout << "ERROR\n";
    }
}

//////////////////////////////////////////////////////////////
// MAIN MENU
//////////////////////////////////////////////////////////////
int main()
{
    initConsole();

    vector<string> text;
    vector<unsigned short> data;

    int choice;

    do
    {
        cout << "\n====== MENU ======\n";
        cout << "1 - Input text\n";
        cout << "2 - Encode\n";
        cout << "3 - Read file\n";
        cout << "4 - Check CRC\n";
        cout << "0 - Exit\n";
        cout << "Choice: ";

        cin >> choice;

        if (choice == 1)
        {
            text = inputText();
        }
        else if (choice == 2)
        {
            data = encode(text);
        }
        else if (choice == 3)
        {
            data = readBinary();
        }
        else if (choice == 4)
        {
            checkCRC(data);
        }

    } while (choice != 0);

    return 0;
}

