#include <iostream>
#include <vector>
#include <cmath>
#include "./solve_system.cpp"

using namespace std;

class factor {

public:

  double coefficient;
  double constant_term;
  int exponent;
  
  // vector containing solutions, from largest denominator power to smallest
  vector<double> solutions;

  factor(double input_coefficient, double input_constant_term, int input_exponent) {
    coefficient = input_coefficient;
    constant_term = input_constant_term;
    exponent = input_exponent;
  }

  // get the value of the factor given an input, there is also the option to provide a different exponent
  double get_value(double input, bool alternative = false, int alternative_exponent = 0) {
    if (alternative) {
      return pow((coefficient * input + constant_term), alternative_exponent);
    } else {
      return pow((coefficient * input + constant_term), exponent);
    }
  }

};

class decomposition {

public:

	int order;

	decomposition() {
		order = 0;
		numerator_setup = false;
		denominator_setup = false;
	}
	
	void add_denominator_factor(double coefficient, double constant_term, int exponent) {
		denominator.push_back(factor(coefficient, constant_term, exponent));
		order += exponent;
	}
	
	void finished_denominator() {
		if (denominator.size() > 0) {
			denominator_setup = true;
			define_system_dimension(order - denominator.size());
		} else {
			throw runtime_error("decomposition.finished_denominator()");
		}
	}
	
	void add_numerator_term(double value) {
		if (denominator_setup && numerator.size() < order) {
			numerator.push_back(value);
		} else {
			throw runtime_error("decomposition.add_numerator_term()");
		}
	}
	
	void finished_numerator() {
		if (denominator_setup && numerator.size() == order) {
			numerator_setup = true;
		} else {
			throw runtime_error("decomposition.finished_numerator()");
		}
	}
	
	void decompose() {
		if (numerator_setup) {
			find_trivials();
			populate_system();
			solve_system();
			extract_solutions();
		} else {
			throw runtime_error("decomposition.decompose()");
		}
	}
	
	void collate_solutions(vector<double>& solutions) {
		for (int i = 0; i < denominator.size(); i++) {
			solutions.insert(solutions.end(), denominator[i].solutions.begin(), denominator[i].solutions.end());
		}
	}

private:

	bool denominator_setup;
	bool numerator_setup;
	
	vector<double> numerator;
	
	vector<factor> denominator;
		
	linear_system my_system;
	
	// returns product of all denominator factors for a given input
	// with the option to change one factor's exponent
	double get_product(double input, int exclusion_index, int exclusion_exponent) {
		double product = 1.0;
		for (int i = 0; i < denominator.size(); i++) {
			if (i == exclusion_index) {
				product *= denominator[i].get_value(input, true, exclusion_exponent);
			} else {
				product *= denominator[i].get_value(input);
			}
		}
		return product;
	}
	
	// returns value of polynomial after substituting an input
	double get_polynomial_value(double input) {
		double value = 0.0;
		for (int i = 0; i < numerator.size(); i++) {
			value += numerator[i] * pow(input, i);
		}
		return value;
	}
	
	// returns the value which will augment a certain row of the matrix
	double generate_row_end(double input) {
		double row_end = get_polynomial_value(input);
		for (int i = 0; i < denominator.size(); i++) {
			row_end -= get_product(input, i, 0) * denominator[i].solutions[0];
		}
		return row_end;
	}
	
	// generate the subrow containing the coefficients for a specific factor
	void generate_subrow(vector<double>& subrow, int subrow_factor, double input) {
		for (int i = 1; i < denominator[subrow_factor].exponent; i++) {
			subrow.push_back(get_product(input, subrow_factor, i));
		}
	}
	
	// collate the subrows and the augmentation to form the full row
	void generate_row(vector<double>& row, double input) {
		for (int i = 0; i < denominator.size(); i++) {
			vector<double> subrow;
			generate_subrow(subrow, i, input);
			row.insert(row.end(), subrow.begin(), subrow.end());
		}
		row.push_back(generate_row_end(input));
	}
	
	// find solutions which do not require the linear system
	void find_trivials() {
		for (int i = 0; i < denominator.size(); i++) {
			double input = -(denominator[i].constant_term / denominator[i].coefficient);
			denominator[i].solutions.push_back(get_polynomial_value(input) / get_product(input, i, 0));
		}
	}
	
	// put the rows into the linear system
	void populate_system() {
		bool system_full = false;
		for (int i = 0; !system_full; i++) {
			vector<double> row;
			double input = random(i);
			generate_row(row, input);
			system_full = my_system.append_row(row);
		}
	}
	
	void solve_system() {
		my_system.solve();
	}
	
	void define_system_dimension(int dimension) {
		my_system.define_dimension(dimension);
	}
	
	void extract_solutions() {
		vector<double> system_solutions;
		my_system.get_solutions(system_solutions);
		int solutions_index = 0;
		for (int i = 0; i < denominator.size(); i++) {
			auto start = system_solutions.begin() + solutions_index;
			auto end = start + denominator[i].exponent - 1;
			denominator[i].solutions.insert(denominator[i].solutions.end(), start, end);
			solutions_index += denominator[i].exponent - 1;
		}
	}
	
	double random(double input) {
		return pow(input + 6.74, 2);
	}

};