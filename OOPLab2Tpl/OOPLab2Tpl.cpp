#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

const int ROWS = 16;
const int COLS = 4;

int parity(unsigned short x, int l, int r)
{
    int p = 0;
    for (int i = l; i <= r; i++) p ^= (x >> i) & 1;
    return p;
}

// ---------------- TASK 1 ----------------
// 2049*b + (15*d + 12*a)/2048 - 100*c + 104*d
void task1()
{
    int a, b, c, d;
    cout << "Enter a b c d: ";
    cin >> a >> b >> c >> d;

    int res = ((b << 11) + b)
        + ((((d << 4) - d) + ((a << 3) + (a << 2))) >> 11)
        - ((c << 6) + (c << 5) + (c << 2))
        + ((d << 6) + (d << 5) + (d << 3));

    cout << "Result = " << res << endl;
}

// ---------------- INPUT ----------------
vector<string> inputText()
{
    vector<string> s(ROWS);
    cin.ignore();
    cout << "Enter 16 lines (up to 4 chars):\n";
    for (int i = 0; i < ROWS; i++)
    {
        getline(cin, s[i]);
        if (s[i].size() > 4) s[i] = s[i].substr(0, 4);
        while (s[i].size() < 4) s[i] += ' ';
    }
    return s;
}

// ---------------- TASK 2 ----------------
unsigned short encode(int row, int pos, unsigned char ch)
{
    unsigned short w = 0;
    int hi = (ch >> 4) & 15;
    int lo = ch & 15;

    w |= row;                 // 0-3
    w |= hi << 4;             // 4-7
    w |= parity(w, 0, 7) << 8;
    w |= lo << 9;             // 9-12
    w |= pos << 13;           // 13-14
    w |= parity(w, 9, 14) << 15;

    return w;
}

void encryptBitwise()
{
    vector<string> s = inputText();
    ofstream f("bitwise.bin", ios::binary);

    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
        {
            unsigned short w = encode(i, j, s[i][j]);
            f.write((char*)&w, sizeof(w));
        }

    f.close();
    cout << "Saved to bitwise.bin\n";
}

void decryptBitwise()
{
    ifstream f("bitwise.bin", ios::binary);
    if (!f) { cout << "File not found\n"; return; }

    vector<string> s(ROWS, "    ");

    for (int k = 0; k < ROWS * COLS; k++)
    {
        unsigned short w;
        f.read((char*)&w, sizeof(w));

        int row = w & 15;
        int hi = (w >> 4) & 15;
        int p1 = (w >> 8) & 1;
        int lo = (w >> 9) & 15;
        int pos = (w >> 13) & 3;
        int p2 = (w >> 15) & 1;

        int c1 = parity(w & ~(1 << 8), 0, 7);
        int c2 = parity(w & ~(1 << 15), 9, 14);

        if (p1 != c1 || p2 != c2)
            cout << "Parity error in word " << k + 1 << endl;

        s[row][pos] = char((hi << 4) | lo);
    }

    f.close();

    ofstream out("decoded_bitwise.txt");
    for (int i = 0; i < ROWS; i++)
    {
        cout << s[i] << endl;
        out << s[i] << endl;
    }
    out.close();
}

// ---------------- TASK 3 ----------------
struct Bits
{
    unsigned row : 4;
    unsigned hi : 4;
    unsigned p1 : 1;
    unsigned lo : 4;
    unsigned pos : 2;
    unsigned p2 : 1;
};

union Word
{
    Bits b;
    unsigned short value;
};

void encryptStruct()
{
    vector<string> s = inputText();
    ofstream f("struct.bin", ios::binary);

    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
        {
            Word w;
            w.value = 0;
            w.b.row = i;
            w.b.hi = (s[i][j] >> 4) & 15;
            w.b.p1 = parity(w.value, 0, 7);
            w.b.lo = s[i][j] & 15;
            w.b.pos = j;
            w.b.p2 = parity(w.value, 9, 14);
            f.write((char*)&w.value, sizeof(w.value));
        }

    f.close();
    cout << "Saved to struct.bin\n";
}

void decryptStruct()
{
    ifstream f("struct.bin", ios::binary);
    if (!f) { cout << "File not found\n"; return; }

    vector<string> s(ROWS, "    ");

    for (int k = 0; k < ROWS * COLS; k++)
    {
        Word w;
        f.read((char*)&w.value, sizeof(w.value));

        int c1 = parity(w.value & ~(1 << 8), 0, 7);
        int c2 = parity(w.value & ~(1 << 15), 9, 14);

        if (w.b.p1 != c1 || w.b.p2 != c2)
            cout << "Parity error in word " << k + 1 << endl;

        s[w.b.row][w.b.pos] = char((w.b.hi << 4) | w.b.lo);
    }

    f.close();

    ofstream out("decoded_struct.txt");
    for (int i = 0; i < ROWS; i++)
    {
        cout << s[i] << endl;
        out << s[i] << endl;
    }
    out.close();
}

// ---------------- MENU ----------------
int main()
{
    int ch;
    do
    {
        cout << "\n1 - Expression\n";
        cout << "2 - Encrypt bitwise\n";
        cout << "3 - Decrypt bitwise\n";
        cout << "4 - Encrypt struct/union\n";
        cout << "5 - Decrypt struct/union\n";
        cout << "0 - Exit\n";
        cout << "Choice: ";
        cin >> ch;

        if (ch == 1) task1();
        else if (ch == 2) encryptBitwise();
        else if (ch == 3) decryptBitwise();
        else if (ch == 4) encryptStruct();
        else if (ch == 5) decryptStruct();

    } while (ch != 0);

    return 0;
}
