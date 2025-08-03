#include <iostream>
#include <vector>
#include "./part_frac.cpp"

using namespace std;

void get_denominator(decomposition& my_decomp);
void get_numerator(decomposition& my_decomp);
void print_solutions(vector<double>& solutions);

int main() {

	decomposition my_decomp;
	
	get_denominator(my_decomp);
	get_numerator(my_decomp);
	
	my_decomp.decompose();
	
	vector<double> solutions;
	my_decomp.collate_solutions(solutions);
	print_solutions(solutions);
	
}

void get_denominator(decomposition& my_decomp) {
	string buffer = "";
	cout << "Please enter the denominator's factors.\nPut them in the form (ax + b)^c, please enter a, then a comma, followed by b, then c. Press return to enter the next. Type E to signal end.\n";
	while (buffer != "E") {
		getline(cin, buffer);
		try {
			int comma_position = buffer.find(",");
			double coefficient = stod(buffer.substr(0, comma_position));
			buffer = buffer.substr(comma_position + 1);
			comma_position = buffer.find(",");
			double constant_term = stod(buffer.substr(0, comma_position));
			int exponent = stoi(buffer.substr(comma_position + 1));
			my_decomp.add_denominator_factor(coefficient, constant_term, exponent);
		} catch(...) { // in case of unexpected input just continue to next iteration
			continue;
		}
	}
	my_decomp.finished_denominator();
}

void get_numerator(decomposition& my_decomp) {
	string buffer = "";
	cout << "Please enter the coefficients of the numerator polynomial, starting from the constant term and continuing with increasing exponents.\n";
	for (int i = 0; i < my_decomp.order; i++) {
		getline(cin, buffer);
		try {
			my_decomp.add_numerator_term(stod(buffer));
		} catch(...) {
			my_decomp.add_numerator_term(0.0);
		}
	}
	my_decomp.finished_numerator();
}

void print_solutions(vector<double>& solutions) {
	for (int i = 0; i < solutions.size(); i++) {
		cout << "solutions[" << i << "] = " << solutions[i] << '\n';
	}
}