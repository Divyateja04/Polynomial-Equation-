#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <cstdint>
#include <stdexcept>

using json = nlohmann::json;
using namespace std;

// Function to decode a value from a given base to decimal
long long decode_value(const string& value, int base) {
    long long result = 0;
    for (char c : value) {
        result *= base;
        if (c >= '0' && c <= '9') {
            result += c - '0';
        } else if (c >= 'a' && c <= 'f') {
            result += 10 + (c - 'a');
        } else if (c >= 'A' && c <= 'F') {
            result += 10 + (c - 'A');
        } else {
            throw runtime_error("Invalid character in value: " + string(1, c));
        }
    }
    return result;
}

// Function to compute Lagrange interpolation at x = 0 to find the constant term
long long lagrange_interpolation(const vector<long long>& x, const vector<long long>& y, int k) {
    long long result = 0;
    for (int i = 0; i < k; ++i) {
        long long term = y[i];
        for (int j = 0; j < k; ++j) {
            if (i != j) {
                // For x = 0, the Lagrange basis polynomial is: l_i(0) = product((0 - x_j)/(x_i - x_j)) for j != i
                // Since numerator is 0, we only need denominator
                term *= -x[j];
                term /= (x[i] - x[j]);
            }
        }
        result += term;
    }
    return result;
}

int main() {
    try {
        // Read JSON from file
        ifstream file("input.json");
        if (!file.is_open()) {
            throw runtime_error("Cannot open input.json");
        }
        json input;
        file >> input;

        // Extract n and k
        int n = input["keys"]["n"];
        int k = input["keys"]["k"];

        // Store points (x, y)
        vector<long long> x_values;
        vector<long long> y_values;

        // Parse each point
        for (const auto& [key, value] : input.items()) {
            if (key != "keys") {
                int x = stoi(key);
                int base = stoi(value["base"].get<string>());
                string y_value = value["value"];
                long long y = decode_value(y_value, base);
                x_values.push_back(x);
                y_values.push_back(y);
            }
        }

        // Ensure we have enough points
        if (x_values.size() < k) {
            throw runtime_error("Not enough points to reconstruct polynomial");
        }

        // Use first k points for interpolation
        long long secret = lagrange_interpolation(x_values, y_values, k);

        // Output the secret
        cout << "The secret (constant term) is: " << secret << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}