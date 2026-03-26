#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int parity(unsigned short x, int l, int r)
{
    int p = 0;
    for (int i = l; i <= r; i++) p ^= (x >> i) & 1;
    return p;
}

// ---------------- TASK 1 ----------------
void task1()
{
    int a, b, c, d;
    cout << "Enter a b c d: ";
    cin >> a >> b >> c >> d;

    int result =
        ((b << 11) + b) +                                  
        ((((d << 4) - d) + ((a << 3) + (a << 2))) >> 11) - 
        ((c << 6) + (c << 5) + (c << 2)) +               
        ((d << 6) + (d << 5) + (d << 3));                  

    cout << "Result = " << result << endl;
}

// ---------------- TASK 2 ENCRYPT ----------------
unsigned short encodeChar(int row, int pos, unsigned char ch)
{
    unsigned short w = 0;
    int hi = (ch >> 4) & 15;
    int lo = ch & 15;

    w |= row;          // bits 0-3
    w |= hi << 4;      // bits 4-7
    w |= parity(w, 0, 7) << 8;   // bit 8
    w |= lo << 9;      // bits 9-12
    w |= pos << 13;    // bits 13-14
    w |= parity(w, 9, 14) << 15; // bit 15

    return w;
}

void encryptText()
{
    cin.ignore();
    ofstream fout("data.bin", ios::binary);

    cout << "Enter 16 lines (up to 4 chars):\n";
    for (int i = 0; i < 16; i++)
    {
        string s;
        getline(cin, s);
        while (s.size() < 4) s += ' ';
        if (s.size() > 4) s = s.substr(0, 4);

        for (int j = 0; j < 4; j++)
        {
            unsigned short w = encodeChar(i, j, s[j]);
            fout.write((char*)&w, sizeof(w));
        }
    }

    fout.close();
    cout << "Encrypted to data.bin\n";
}

// ---------------- TASK 3 DECRYPT ----------------
void decryptText()
{
    ifstream fin("data.bin", ios::binary);
    if (!fin)
    {
        cout << "File not found\n";
        return;
    }

    vector<string> text(16, "    ");

    for (int i = 0; i < 64; i++)
    {
        unsigned short w;
        fin.read((char*)&w, sizeof(w));

        int row = w & 15;
        int hi = (w >> 4) & 15;
        int p1 = (w >> 8) & 1;
        int lo = (w >> 9) & 15;
        int pos = (w >> 13) & 3;
        int p2 = (w >> 15) & 1;

        int c1 = parity(w & ~(1 << 8), 0, 7);
        int c2 = parity(w & ~(1 << 15), 9, 14);

        if (p1 != c1 || p2 != c2)
            cout << "Parity error in word " << i + 1 << endl;

        char ch = (hi << 4) | lo;
        text[row][pos] = ch;
    }

    fin.close();

    ofstream fout("decoded.txt");
    for (int i = 0; i < 16; i++)
    {
        cout << text[i] << endl;
        fout << text[i] << endl;
    }
    fout.close();
}

// ---------------- TASK 4 CRC ----------------
void checkCRC()
{
    int n;
    cout << "How many words? ";
    cin >> n;

    for (int k = 0; k < n; k++)
    {
        unsigned short w;
        cout << "Enter word " << k + 1 << ": ";
        cin >> w;

        int odd = 0, even = 0;
        for (int i = 0; i < 14; i++)
        {
            if (i % 2 == 0) even ^= (w >> i) & 1;
            else odd ^= (w >> i) & 1;
        }

        int b14 = (w >> 14) & 1;
        int b15 = (w >> 15) & 1;

        if (b14 == odd && b15 == even)
            cout << "Word " << k + 1 << ": OK\n";
        else
            cout << "Word " << k + 1 << ": ERROR\n";
    }
}

// ---------------- MAIN ----------------
int main()
{
    int ch;
    do
    {
        cout << "\n1 - Expression\n";
        cout << "2 - Encrypt text\n";
        cout << "3 - Decrypt text\n";
        cout << "4 - Check CRC\n";
        cout << "0 - Exit\n";
        cout << "Choice: ";
        cin >> ch;

        if (ch == 1) task1();
        else if (ch == 2) encryptText();
        else if (ch == 3) decryptText();
        else if (ch == 4) checkCRC();

    } while (ch != 0);

    return 0;
}

    } while (choice != 0);

    return 0;
}

