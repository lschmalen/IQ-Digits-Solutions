#include <iostream>
#include <string>
#include <array>
#include <system_error>
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
    A | B | C | C,
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
    case -1:
      return rgb(0, 0, 0);
      break;
    case 0:
      return rgb(255, 0, 0);
      break;
    case 1:
      return  rgb(255, 165, 0);
      break;
    case 2:
      return rgb(255, 255, 0);
      break;
    case 3:
      return rgb(144, 238, 144);
      break;
    case 4:
      return rgb(0, 100, 0);
      break;
    case 5:
      return rgb(0, 255, 255);
      break;
    case 6:
        return rgb(70, 130, 180);
        break;
    case 7:
        return rgb(30, 70, 200);
        break;
    case 8:
      return rgb(148, 0, 211);
      break;
    case 9:
      return rgb(255, 105, 180);
      break;
        default: return reset;
  }
}

int test_board[9][6] = {
    {9, 9, 1, 1, 3, -2},
    {9, 9, 9, 4, 4, 3},
    {9, 5, 0, 4, 3, -2},
    {-1, 5, 0, 0, 4, 3},
    {8, 5, 0, 2, 3, -2},
    {8, 8, 5, 2, 2, 2},
    {8, 5, 6, 6, 2, -2},
    {8, 8, 6, 6, 6, 7},
    {8, -1, 6, 7, 7, -2}
};

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



int zero1[5][3] = {
    {0,-1,-1},
    {0,0,-1},
    {0,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int one1[5][3] = {
    {1,1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int one2[5][3] = {
    {-1,-1,-1},
    {1,-1,-1},
    {-1,-1,-1},
    {1,-1,-1},
    {-1,-1,-1}
};

int two1[5][3] = {
    {2,-1,-1},
    {2,2,2},
    {-1,2,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int two2[5][3] = {
    {2,-1,-1},
    {-1,2,-1},
    {2,-1,-1},
    {2,-1,-1},
    {2,-1,-1}
};

int three1[5][3] = {
    {3,-1,-1},
    {-1,3,-1},
    {3,-1,-1},
    {-1,3,-1},
    {3,-1,-1}
};

int three2[5][3] = {
    {-1,-1,-1},
    {3,3,3},
    {3,3,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int three3[5][3] = {
    {3,-1,-1},
    {3,-1,-1},
    {3,-1,-1},
    {3,-1,-1},
    {3,-1,-1}
};

int three4[5][3] = {
    {3,3,-1},
    {3,3,3},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};


int four1[5][3] = {
    {-1,-1,-1},
    {4,4,-1},
    {4,-1,-1},
    {-1,4,-1},
    {-1,-1,-1}
};

int four2[5][3] = {
    {4,4,-1},
    {-1,4,-1},
    {4,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int four3[5][3] = {
    {-1,-1,-1},
    {4,-1,-1},
    {4,-1,-1},
    {4,4,-1},
    {-1,-1,-1}
};

int four4[5][3] = {
    {-1,4,-1},
    {-1,4,-1},
    {4,4,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int five1[5][3] = {
    {5,-1,-1},
    {5,-1,-1},
    {5,-1,-1},
    {-1,5,-1},
    {5,-1,-1}
};

int five2[5][3] = {
    {-1,5,-1},
    {5,5,5},
    {5,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};


int six1[5][3] = {
    {6,-1,-1},
    {6,-1,-1},
    {6,-1,-1},
    {6,6,-1},
    {6,-1,-1}
};

int six2[5][3] = {
    {-1,6,-1},
    {6,6,6},
    {6,6,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int six3[5][3] = {
    {6,-1,-1},
    {6,6,-1},
    {6,-1,-1},
    {-1,6,-1},
    {6,-1,-1}
};

int six4[5][3] = {
    {6,6,-1},
    {6,6,6},
    {6,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};


int seven1[5][3] = {
    {7,-1,-1},
    {-1,7,-1},
    {-1,-1,-1},
    {-1,7,-1},
    {-1,-1,-1}
};

int seven2[5][3] = {
    {7,7,-1},
    {7,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int seven3[5][3] = {
    {-1,-1,-1},
    {7,-1,-1},
    {-1,-1,-1},
    {7,-1,-1},
    {7,-1,-1}
};

int seven4[5][3] = {
    {-1,-1,-1},
    {-1,-1,7},
    {7,7,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int eight1[5][3] = {
    {8,-1,-1},
    {8,8,-1},
    {8,-1,-1},
    {8,8,-1},
    {8,-1,-1}
};

int eight2[5][3] = {
    {8,8,-1},
    {8,8,8},
    {8,8,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int nine1[5][3] = {
    {9,-1,-1},
    {9,-1,-1},
    {9,-1,-1},
    {9,9,-1},
    {9,-1,-1}
};

int nine2[5][3] = {
    {-1,9,-1},
    {9,9,9},
    {9,9,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

int nine3[5][3] = {
    {9,-1,-1},
    {9,9,-1},
    {9,-1,-1},
    {-1,9,-1},
    {9,-1,-1}
};

int nine4[5][3] = {
    {9,9,-1},
    {9,9,9},
    {9,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1}
};

//int rotations[10] = {1,2, 2, 4, 4, 2, 4, 4, 2, 4};


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
            // TODO: handle rotations

        }
    }
}



bool place_number(int number, int rotation, int row, int column) {
    bool success = true;

    if (rotation >= rotations[number])
        return false;

    int (*narr)[5][3];

    int value = number * 10 + rotation;
    switch (value) {
        case 00: narr = &zero1; break;
        case 10: narr = &one1; break;
        case 11: narr = &one2; break;
        case 20: narr = &two1; break;
        case 21: narr = &two2; break;
        case 30: narr = &three1; break;
        case 31: narr = &three2; break;
        case 32: narr = &three3; break;
        case 33: narr = &three4; break;
        case 40: narr = &four1; break;
        case 41: narr = &four2; break;
        case 42: narr = &four3; break;
        case 43: narr = &four4; break;
        case 50: narr = &five1; break;
        case 51: narr = &five2; break;
        case 60: narr = &six1; break;
        case 61: narr = &six2; break;
        case 62: narr = &six3; break;
        case 63: narr = &six4; break;
        case 70: narr = &seven1; break;
        case 71: narr = &seven2; break;
        case 72: narr = &seven3; break;
        case 73: narr = &seven4; break;
        case 80: narr = &eight1; break;
        case 81: narr = &eight2; break;
        case 90: narr = &nine1; break;
        case 91: narr = &nine2; break;
        case 92: narr = &nine3; break;
        case 93: narr = &nine4; break;
        default: return false;
    }

    // first check if there is no conflict in placing the number
    for (auto i = 0; i < 5; i++) {
        for (auto j = 0; j < 3; j++) {
            int ii = 2 * row + i;
            int jj = column + j;
            if (ii < 9 && jj < 6) {  // we are within the array
                if ((*narr)[i][j] > -1 && the_board[ii][jj] != -1) { // board is not empty when trying to put number
                    success = false;
                    break;
                }
            } else { // we are outside the board
                if ((*narr)[i][j] != -1) {
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
                    if ((*narr)[i][j] != -1 && the_board[ii][jj] == -1) { //
                        the_board[ii][jj] = (*narr)[i][j];
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
                }
                // remove number before trying to place next number
                remove_number(number);
            }
        }

    }
}


int main() {
    // find_solutions(0);
    init_numbers();
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            cout << numbers[9,0,i,j] << " ";
        }
        cout << endl;

    }

}
