//TODO maybe:
// - add a function to check whether a decomposition is correct, maybe output differences between values on either side of equation

#include <iostream>
#include <vector>
#include <cmath>
#include "./solve_system.cpp"

class factor {

public:

  double coefficient;
  double constant_term;
  int exponent;

  factor(double input_coefficient, double input_constant_term, int input_exponent) {
    coefficient = input_coefficient;
    constant_term = input_constant_term;
    exponent = input_exponent;
  }

  double get_value(double input, bool alternative = false, int alternative_exponent = 0) {
    if (alternative) {
      return pow((coefficient * input + constant_term), alternative_exponent);
    } else {
      return pow((coefficient * input + constant_term), exponent);
    }
  }

};

double get_factor_product(vector<factor>& factors, double input, int exclusion_factor, int exclusion_exponent);
double get_polynomial_value(vector<double>& polynomial, double input);
double generate_row_end(vector<factor>& factors, vector<double>& polynomial, vector<double>& solution, double input);
void generate_subrow(vector<factor>& factors, vector<double>& subrow, int subrow_factor, double input);
void generate_row(vector<factor>& factors, vector<double>& row, double input, int order);


int main() {

  vector<factor> denominator;
  int order = 0;
  string buffer = "";

  cout << "Where factors are in the form (ax + b)^c, please enter a, then a comma, followed by b, then c. Press return to enter the next. Type E to signal end.\n";

  while (buffer != "E") {
    getline(cin, buffer);
    try {

      // extract the values
      int comma_position = buffer.find(",");
      double coefficient = stod(buffer.substr(0, comma_position));
      buffer = buffer.substr(comma_position + 1);
      comma_position = buffer.find(",");
      double constant_term = stod(buffer.substr(0, comma_position));
      int exponent = stoi(buffer.substr(comma_position + 1));
      denominator.push_back(factor(coefficient, constant_term, exponent));

      // add the exponent so we can figure out the order of the denominator
      order += exponent;

    } catch(...) { // in case of unexpected input just continue to next iteration
      continue;
    }

  }

  vector<double> numerator;

  cout << "Please enter the coefficients of the numerator polynomial, starting from the constant term and continuing with increasing exponents.\n";
  for (int i = 0; i < order; i++) {
    getline(cin, buffer);
    try {
      numerator.push_back(stod(buffer));
    } catch(...) {
      numerator.push_back(0.0);
    }
  }

  for (int i = 0; i < denominator.size(); i++) {
    cout << "coefficient = " << denominator[i].coefficient << ", constant term = " << denominator[i].constant_term << ", exponent = " << denominator[i].exponent << "\n";
  }

  for (int i = 0; i < numerator.size(); i++) {
    cout << "x^" << i << " coefficient = " << numerator[i] << "\n";
  }

  vector<double> solution;

  for (int i = 0; i < order; i++) {
    solution.push_back(NAN);
  }

  int cumulative_order = -1;

  for (int i = 0; i < denominator.size(); i++) {
    double input = -1.0 * (denominator[i].constant_term / denominator[i].coefficient);
    cout << "i = " << i << ", input = " << input << ", polyval = " << get_polynomial_value(numerator, input) << ", factprod = " << get_factor_product(denominator, input, i, 0) << "\n";
    solution[cumulative_order + denominator[i].exponent] = get_polynomial_value(numerator, input) / get_factor_product(denominator, input, i, 0);
    cumulative_order += denominator[i].exponent;
  }

  for (int i = 0; i < solution.size(); i++) {
    cout << "solution[" << i << "] = " << solution[i] << "\n";
  }

  linear_system my_system(order - denominator.size());
  cout << "Just created system with size: " << order - denominator.size() << "\n";

  for (int i = 0; true; i++) {
    vector<double> row;
    double input = pow(i + 6.75, 2);
    generate_row(denominator, row, input, order);
    for (int a = 0; a < row.size(); a++) {
      cout << "unended row[" << a << "] = " << row[a] << "\n";
    }
    row.push_back(generate_row_end(denominator, numerator, solution, input));
    for (int a = 0; a < row.size(); a++) {
      cout << "row[" << a << "] = " << row[a] << "\n";
    }
    //row.push_back(get_polynomial_value(numerator, input));
    try {
      my_system.append_row(row);
    } catch(int exception) {
      cout << "Exception thrown: " << exception << "\n";
      if (exception == 3) {
        continue;
      } else if (2) {
        break;
      } else {
        throw 1;
      }
    }
  }

  cout << "Matrix, pre-solve:\n";
  my_system.print_matrix();

  try {
    my_system.solve();
  } catch(...) {
    cout << "Error when solving linear system.";
    throw 1;
  }

  cout << "Matrix, post-solve:\n";
  my_system.print_matrix();

  vector<double> system_solutions;
  my_system.get_solutions(system_solutions);

  int difference = 0;
  for (int i = 0; i < order; i++) {
    if (isnan(solution[i])) {
      solution[i] = system_solutions[i - difference];
    } else {
      difference++;
    }
  }

  // TODO the problem with the code right now is that when you build the linear system, you have to subtract the known constants * their respective factors.

  for (int i = 0; i < solution.size(); i++) {
    cout << "solution[" << i << "] = " << solution[i] << "\n";
  }

}

double get_factor_product(vector<factor>& factors, double input, int exclusion_factor, int exclusion_exponent) {
  double product = 1.0;
  for (int i = 0; i < factors.size(); i++) {
    if (i == exclusion_factor) {
      product *= factors[i].get_value(input, true, exclusion_exponent);
    } else {
      product *= factors[i].get_value(input);
    }
  }
  return product;
}

double get_polynomial_value(vector<double>& polynomial, double input) {
  double value = 0.0;
  for (int i = 0; i < polynomial.size(); i++) {
    value += polynomial[i] * pow(input, i);
  }
  return value;
}

double generate_row_end(vector<factor>& factors, vector<double>& polynomial, vector<double>& solution, double input) {
  double row_end = get_polynomial_value(polynomial, input);
  vector<double> solution_copy;
  for (int i = 0; i < solution.size(); i++) {
    if (!isnan(solution[i])) {
      solution_copy.push_back(solution[i]);
    }
  }
  if (solution_copy.size() != factors.size()) cout << "ERROR SIZES NOT EQUAL\n";
  for (int i = 0; i < factors.size(); i++) {
    row_end -= get_factor_product(factors, input, i, 0) * solution_copy[i];
  }
  return row_end;
}

void generate_subrow(vector<factor>& factors, vector<double>& subrow, int subrow_factor, double input) {
  for (int i = 1; i < factors[subrow_factor].exponent; i++) {
    subrow.push_back(get_factor_product(factors, input, subrow_factor, factors[subrow_factor].exponent - i));
  }
}

void generate_row(vector<factor>& factors, vector<double>& row, double input, int order) {
  for (int i = 0; i < factors.size(); i++) {
    vector<double> subrow;
    generate_subrow(factors, subrow, i, input);
    row.insert(row.end(), subrow.begin(), subrow.end());
  }
}
