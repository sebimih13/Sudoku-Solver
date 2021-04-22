#include "SudokuSolver.h"

#define r first
#define c second

SudokuSolver::SudokuSolver()
{
    Table.resize(MaxNr, std::vector<int>(MaxNr));
    Column.resize(MaxNr, std::vector<bool>(MaxNr));
    Row.resize(MaxNr, std::vector<bool>(MaxNr));
    Square.resize(MaxNr, std::vector<bool>(MaxNr));

    FoundSolution = false;

    // todo : delete debug
    Table[1][1] = 1;
    Table[1][2] = 2;
    Table[1][3] = 3;
    Table[1][4] = 4;
    Table[1][5] = 5;
    Table[1][6] = 6;
    Table[1][7] = 7;
    Table[1][8] = 8;
    Table[1][9] = 9;
}

int SudokuSolver::GetSquareNumber(int row, int column)
{
    /*
    
        1...3 (1) si 1...3 (0) => 1
        1...3 (1) si 4...6 (1) => 2
        1...3 (1) si 7...9 (2) => 3
        
        4...6 (4) si 1...3 (0) => 4
        4...6 (4) si 4...6 (1) => 5
        4...6 (4) si 7...9 (2) => 6

        7...9 (7) si 1...3 (0) => 7
        7...9 (7) si 4...6 (1) => 8
        7...9 (7) si 7...9 (2) => 9

    */

    // return (row - 1) / 3 * 3 + 1 + (column - 1) / 3;

    // todo : delete
    if (row <= 3)
    {
        if (column <= 3)
        {
            return 1;
        }
        else if (column <= 6)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }
    else if (row <= 6)
    {
        if (column <= 3)
        {
            return 4;
        }
        else if (column <= 6)
        {
            return 5;
        }
        else
        {
            return 6;
        }
    }
    else
    {
        if (column <= 3)
        {
            return 7;
        }
        else if (column <= 6)
        {
            return 8;
        }
        else
        {
            return 9;
        }
    }
}

bool SudokuSolver::CanPlace(std::pair<int, int> pos, int value)
{
    return !Row[pos.r][value]
        && !Column[pos.c][value]
        && !Square[GetSquareNumber(pos.r, pos.c)][value];
}

void SudokuSolver::MarkPlaced(std::pair<int, int> pos, int value, bool placed)
{
    Row[pos.r][value] = Column[pos.c][value] = Square[GetSquareNumber(pos.r, pos.c)][value] = placed;
}

void SudokuSolver::bkt(int level)
{
    if (level == EmptySpaces.size())
    {
        FoundSolution = true;
        return;
    }

    for (int i = 1; i < MaxNr; i++)
    {
        if (CanPlace(EmptySpaces[level], i))
        {
            Table[EmptySpaces[level].r][EmptySpaces[level].c] = i;
            MarkPlaced(EmptySpaces[level], i, true);

            bkt(level + 1);
            if (FoundSolution)
                return;

            Table[EmptySpaces[level].r][EmptySpaces[level].c] = 0;
            MarkPlaced(EmptySpaces[level], i, false);
        }
    }
}

bool SudokuSolver::Solve()
{
    FoundSolution = false;

    Column.assign(MaxNr, std::vector<bool>(MaxNr, false));
    Row.assign(MaxNr, std::vector<bool>(MaxNr, false));
    Square.assign(MaxNr, std::vector<bool>(MaxNr, false));
    EmptySpaces.clear();

    for (int i = 1; i < MaxNr; i++)
    {
        for (int j = 1; j < MaxNr; j++)
        {
            if (Table[i][j] != 0)
            {
                if (Column[j][Table[i][j]])
                    return false;

                if (Row[i][Table[i][j]])
                    return false;

                if (Square[GetSquareNumber(i, j)][Table[i][j]])
                    return false;

                Column[j][Table[i][j]] = true;
                Row[i][Table[i][j]] = true;
                Square[GetSquareNumber(i, j)][Table[i][j]] = true;
            }
            else
            {
                EmptySpaces.push_back({ i, j });
            }
        }
    }

    bkt(0);
    return FoundSolution;
}

void SudokuSolver::SetTableValue(int row, int column, int value)
{
    Table[row][column] = value;
}

int SudokuSolver::GetTableValue(int row, int column)
{
    return Table[row][column];
}

void SudokuSolver::Clear()
{
    Table.assign(MaxNr, std::vector<int>(MaxNr, 0));
}

