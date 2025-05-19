#include <iostream>
#include <vector>

using namespace std;

class linear_system {

private:

  vector<vector<double> > matrix;
  int dimension;

  void multiply_row(int row, double factor) {
    for (int i = 0; i < matrix[0].size(); i++) {
      matrix[row][i] = factor * matrix[row][i];
    }
  }

  void add_rows(int origin, int recipient, double factor) {
    for (int i = 0; i < matrix[0].size(); i++) {
      matrix[recipient][i] += factor * matrix[origin][i];
    }
  }

  void pivot(int position) {
    int largest = position;
    for (int i = position + 1; i < matrix.size(); i++) {
      if (matrix[i][position] > matrix[largest][position]) {
        largest = i;
      }
    }
    if (matrix[largest][position] != matrix[position][position]) {
      matrix[position].swap(matrix[largest]);
    }
    if (matrix[position][position] == 0.0) {
      throw 4;
    }
  }

  void eliminate(int position) {
    for (int i = 0; i < matrix.size(); i++) {
      if (i != position) {
        double factor = -matrix[i][position] / matrix[position][position];
        add_rows(position, i, factor);
      }
    }
  }

  bool is_linearly_dependent(int row_one, vector<double>& row_two) {
    double factor = matrix[row_one][0] / row_two[0];
    for (int i = 1; i < matrix[0].size(); i++) {
      if (matrix[row_one][i] / row_two[i] != factor) {
        return false;
      }
    }
    return true;
  }

public:

  linear_system(int input_dimension) {
    dimension = input_dimension;
  }

  void append_row(vector<double>& row) {
    if (row.size() != dimension + 1) { // wrong row length error
      throw 1;
    }
    if (matrix.size() == dimension) { // full matrix error
      throw 2;
    }
    for (int i = 0; i < matrix.size(); i++) {
      if (is_linearly_dependent(i, row)) { // linear dependence error
        throw 3;
      }
    }
    matrix.push_back(row);
  }

  void print_matrix() {
    for (int y = 0; y < matrix.size(); y++) {
      for (int x = 0 ; x < matrix[0].size(); x++) {
        cout << matrix[y][x] << " ";
      }
      cout << "\n";
    }
  }

  void solve() {
    for (int i = 0; i < matrix.size(); i++) {
      pivot(i);
      double factor = 1 / matrix[i][i];
      multiply_row(i, factor);
      eliminate(i);
    }
  }

  void get_solutions(vector<double>& solutions) {
    for (int i = 0; i < matrix.size(); i++) {
      solutions.push_back(matrix[i][matrix[i].size() - 1]);
    }
  }

};
