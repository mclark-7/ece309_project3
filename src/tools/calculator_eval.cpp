// src/tools/calculator_eval.cpp — YOU IMPLEMENT (spec §4.6).
//
// calc_evaluate: evaluate a well-formed token list with two stacks
// (one for numbers, one for operators and '('). See spec §4.6 and the
// worked trace in Appendix C. std::stack or std::vector are both fine.

#include "tools/calculator.h"
#include "tools/calculator.h"
#include <stack>
#include <string>
#include <vector>

using namespace std;

// Prioritys
int priority(CalcToken::Kind op) {

    if (op == CalcToken::Kind::Times ||
        op == CalcToken::Kind::Divide) {
        return 2;
    }

    return 1;
}

// Calc
bool calculate(stack<double>& numbers, stack<CalcToken::Kind>& operators, string* error) {

    if (numbers.size() < 2 || operators.empty()) {
        *error = "invalid expression";
        return false;
    }

    double right = numbers.top();
    numbers.pop();
    double left = numbers.top();
    numbers.pop();
    CalcToken::Kind op = operators.top();
    operators.pop();
    double answer = 0;

    if (op == CalcToken::Kind::Plus) {
        answer = left + right;
    }
    else if (op == CalcToken::Kind::Minus) {
        answer = left - right;
    }
    else if (op == CalcToken::Kind::Times) {
        answer = left * right;
    }
    else if (op == CalcToken::Kind::Divide) {

        if (right == 0) {
            *error = "division by zero";
            return false;
        }
        answer = left / right;
    }
    else {
        *error = "invalid operator";
        return false;
    }
    numbers.push(answer);
    return true;
}

// Calc Function
bool calc_evaluate(const vector<CalcToken>& tokens, double* out, string* error) {
    stack<double> numbers;
    stack<CalcToken::Kind> operators;

    for (int i = 0; i < static_cast<int>(tokens.size()); i++) {
        CalcToken token = tokens[i];

        // Save
        if (token.kind == CalcToken::Kind::Number) {
            numbers.push(token.value);
        }
        else if (token.kind == CalcToken::Kind::LParen) {
            operators.push(token.kind);
        }
        else if (token.kind == CalcToken::Kind::RParen) {

            while (!operators.empty() &&
                   operators.top() != CalcToken::Kind::LParen) {

                if (!calculate(numbers, operators, error)) {
                    return false;
                }
            }

            if (operators.empty()) {
                *error = "invalid parentheses";
                return false;
            }
            operators.pop();
        }
        else {
            while (!operators.empty() &&
                   operators.top() != CalcToken::Kind::LParen &&
                   priority(operators.top()) >= priority(token.kind)) {

                if (!calculate(numbers, operators, error)) {
                    return false;
                }
            }
            operators.push(token.kind);
        }
    }

    while (!operators.empty()) {

        if (!calculate(numbers, operators, error)) {
            return false;
        }
    }

    if (numbers.size() != 1) {
        *error = "invalid expression";
        return false;
    }
    *out = numbers.top();
    return true;
}

