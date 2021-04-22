#pragma once

#include <iostream>
#include <vector>

class SudokuSolver
{
public:
	// constructor
	SudokuSolver();

	bool Solve();

	void SetTableValue(int row, int column, int value);
	int GetTableValue(int row, int column);

	void Clear();

private:
	const int MaxNr = 10;
	std::vector<std::vector<int>> Table;
	std::vector<std::vector<bool>> Column, Row, Square;
	std::vector<std::pair<int, int>> EmptySpaces;

	void bkt(int level);
	bool FoundSolution;

	int GetSquareNumber(int row, int column);
	bool CanPlace(std::pair<int, int> pos, int value);
	void MarkPlaced(std::pair<int, int> pos, int value, bool placed);
};

