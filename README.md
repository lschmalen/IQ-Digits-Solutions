# IQ-Digits-Solutions
Very simple C++ program that lists all solutions to the IQ Digits puzzle without taking into account symmetries.

When my son had the game IQ Digits (https://www.smartgames.eu/uk/one-player-games/iq-digits), I was wondering, how many arrangements of numbers there are. The manual already lists 120 possible arrangements as puzzles (maybe some are even identical). I was wondering how many arrangements there really are. SUch puzzles were routinously asked in the IOI (International Olympiad in Informatics) and hence I quickly hacked a small program, which is absolutely not beautiful (all the 7-segment numbers hacked in separately), but does the job. The main algorithm is a simple backtracking recursive function. The program does not take into account symmetries, as the board itself is clearly oriented and hence symmetries are not directly relevant. It would be interesting though to look a bit closer into symmetries.

Compile using `g++ -O -o solutions solutions.cpp` and run simply via `./solutions` without command line arguments. All arrangements are displayed on the terminal.
