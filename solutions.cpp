#include <iostream>
#include <string>
#include <array>
#include <algorithm>
#include <vector>
#include <mdspan>

using namespace std;

string rgb(int r, int g, int b) {
    return "\033[38;2;" + to_string(r) + ";" +
           to_string(g) + ";" + to_string(b) + "m";
}

const string reset = "\033[0m";


// 7-segment display defined as follows
// ----A----
// |       |
// B       C
// |       |
// |---D---|
// |       |
// E       F
// |       |
// |---G---|
enum segment {
  A = 1 << 0, // top
  B = 1 << 1, // upper right
  C = 1 << 2, // lower right
  D = 1 << 3, // bottom
  E = 1 << 4, // lower left
  F = 1 << 5, // upper left
  G = 1 << 6  // middle
};

// define the numbers and which segments are lit
array<int, 10> digit_segments = {
    A | B | C | D,
    B | E,
    A | C | D | E | G,
    A | C | D | F | G,
    B | C | D | F,
    A | B | D | F | G,
    A | B | D | E | F | G,
    A | C | F,
    A | B | C | D | E | F | G,
    A | B | C | D | F | G
};

// number of rotational symmetries
array<int,10> rotations = {1,2, 2, 4, 4, 2, 4, 4, 2, 4};


string get_color(int entry) {
    switch (entry) {
    case -1: return rgb(0, 0, 0);       break;
    case 0:  return rgb(255, 0, 0);     break;
    case 1:  return  rgb(255, 165, 0);  break;
    case 2:  return rgb(255, 255, 0);   break;
    case 3:  return rgb(144, 238, 144); break;
    case 4:  return rgb(0, 100, 0);     break;
    case 5:  return rgb(0, 255, 255);   break;
    case 6:  return rgb(70, 130, 180);  break;
    case 7:  return rgb(30, 70, 200);   break;
    case 8:  return rgb(148, 0, 211);   break;
    case 9:  return rgb(255, 105, 180); break;
    default: return reset;
  }
}


// -1 is empty and -2 is undefined
int the_board[9][6] = {
    {-1, -1, -1, -1, -1, -2},
    {-1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -2},
    {-1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -2},
    {-1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -2},
    {-1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -2}
};



// storage for the numbers, 10 numbers * 4 rotations * 5x3 size of the 7 segment display with 5 rows (3 vertical and 2 horizontal) and 3 columns
vector<int> storage(10 * 4 * 5 * 3, -1);
auto numbers = mdspan(storage.data(), 10, 4, 5, 3);

void init_numbers() {
    for (int number = 0; number < 10; number++) {
        numbers[number, 0, 0, 0] = (digit_segments[number] & A) == 0 ? -1 : number;
        numbers[number, 0, 1, 0] = (digit_segments[number] & B) == 0 ? -1 : number;
        numbers[number, 0, 1, 1] = (digit_segments[number] & C) == 0 ? -1 : number;
        numbers[number, 0, 2, 0] = (digit_segments[number] & D) == 0 ? -1 : number;
        numbers[number, 0, 3, 0] = (digit_segments[number] & E) == 0 ? -1 : number;
        numbers[number, 0, 3, 1] = (digit_segments[number] & F) == 0 ? -1 : number;
        numbers[number, 0, 4, 0] = (digit_segments[number] & G) == 0 ? -1 : number;

        for (int rotation = 1; rotation < rotations[number]; rotation++) {
          // first convert array into a 2d representation of the two neighboring
          // 7-segment parts
            vector<vector<int>> temp(5, vector<int>(5, 0));
            // convert horizontal segments
            for (int row = 0; row < 5; row+=2) {
                for (int col = 0; col < 2; col++) {
                    if (numbers[number, rotation-1, row, col] > -1) temp[row][2*col+1] = 1;
              }
            }
            // cvonert vertical segments
            for (int row = 1; row < 5; row+=2) {
                for (int col = 0; col < 3; col++) {
                    if (numbers[number, rotation-1, row, col] > -1) temp[row][2*col] = 1;
              }
            }
            // first transpose array
            for (int i = 0; i < 5; i++) {
                for (int j = i + 1; j < 5; j++) {
                    swap(temp[i][j], temp[j][i]);
                }
            }
            // then reverse each row
            for (int i = 0; i < 5; i++) {
                reverse(temp[i].begin(), temp[i].end());
            }

            // now check if first two rows are zero and if yes, rotate them to the
            // end
            if (all_of(temp[0].begin(), temp[0].end(), [](int x) { return x == 0; }) && all_of(temp[1].begin(), temp[1].end(), [](int x) { return x == 0; })) {
                    rotate(temp.begin(), temp.begin() + 2, temp.end());
            }

            // now check if the first two columns are  zero and if that is the case, rotate them to the back
            if (all_of(temp.begin(), temp.end(),   [](const std::vector<int> &row) { return row[0] == 0 && row[1] == 0; })) {
                for (auto& row : temp) {
                    rotate(row.begin(), row.begin() + 2, row.end());
                }
            }

            // convert back into compressed version, first horizontal segments
            for (int row = 0; row < 5; row+=2) {
                for (int col = 0; col < 2; col++) {
                    if (temp[row][2*col+1] == 1) numbers[number, rotation, row, col] = number;
              }
            }
            // now vertical segments
            for (int row = 1; row < 5; row+=2) {
                for (int col = 0; col < 3; col++) {
                    if (temp[row][2*col] == 1) numbers[number, rotation, row, col] = number;
              }
            }
        }
    }
}



bool place_number(int number, int rotation, int row, int column) {
    bool success = true;

    if (rotation >= rotations[number])
        return false;


    // first check if there is no conflict in placing the number
    for (auto i = 0; i < 5; i++) {
        for (auto j = 0; j < 3; j++) {
            int ii = 2 * row + i;
            int jj = column + j;
            if (ii < 9 && jj < 6) {  // we are within the array
                if (numbers[number, rotation, i, j] > -1 && the_board[ii][jj] != -1) { // board is not empty when trying to put number
                    success = false;
                    break;
                }
            } else { // we are outside the board
                if (numbers[number, rotation, i, j] != -1) {
                    success = false;
                    break;
                }
            }
        }
        if (!success) break;
    }

    // now place the number
    if (success) {
        for (auto i = 0; i < 5; i++) {
            for (auto j = 0; j < 3; j++) {
                int ii = 2 * row + i;
                int jj = column + j;
                if (ii < 9 && jj < 6) {  // we are within the array
                    if (numbers[number, rotation, i, j] != -1 && the_board[ii][jj] == -1) { //
                        the_board[ii][jj] = numbers[number, rotation, i, j];
                    }
                }
            }
        }
    }
    return success;
}

void remove_number(int number) {
    for (auto row = 0; row < 9; row++)
        for (auto col = 0; col < 6; col++)
            if (the_board[row][col] == number)
                the_board[row][col] = -1;

}


void print_board(const int (&board)[9][6]) {
    for (auto row = 0; row < 8; row+=2) {
        for (auto i = 0; i < 5; i++) {
            cout << " " << get_color(board[row][i]) << "---";
        }
        cout << endl;
        for (auto i = 0; i < 6; i++) {
            cout << get_color(board[row+1][i]) << "|";
            if (i < 5) cout << "   ";
        }
        cout << endl;
    }
    for (auto i = 0; i < 5; i++) {
        cout << " " << get_color(board[8][i]) << "---";
    }
    cout << reset << endl;
}


int total_solutions = 0;

void find_solutions(int number) {
    if (number == 10) {
        total_solutions++;
        cout << "Solution #" << total_solutions << endl;
        print_board(the_board);
        return;
    }
    for (int rotation = 0; rotation < rotations[number]; rotation++) {
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 5; col++) {
                // try to place number
                bool success = place_number(number, rotation, row, col);
                if (success) {
                    find_solutions(number+1);  // try to put next number
                    // remove number before trying to place next number
                    remove_number(number);
                }
            }
        }

    }
}


int main(int argc, char* argv[]) {

    init_numbers();
    find_solutions(0);

}
