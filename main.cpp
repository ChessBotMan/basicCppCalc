#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include <chrono>
#include <stdexcept>
#include <cctype>
#define _USE_MATH_DEFINES 

using namespace std;

enum class TokenType {
    NUMBER,
    OPERATOR,
    FUNCTION,
    LPAREN,
    RPAREN,
    COMMA
};

struct Token {
    TokenType type;
    string name;
    double value;
    int pos;
};

class ParseError : public runtime_error {
public:
    int pos;
    ParseError(const string& msg, int p) : runtime_error(msg), pos(p) {}
};

// Map operators to precedences
int precedence(const string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "~" || op == "#") return 3; // unary minus and plus
    return 0;
}

vector<Token> tokenize(const string& expr) {
    vector<Token> tokens;
    int i = 0;
    int n = expr.length();
    bool expect_operand = true;

    while (i < n) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        if (isalpha(expr[i])) {
            int start = i;
            while (i < n && isalnum(expr[i])) i++;
            string word = expr.substr(start, i - start);
            
            if (word == "pi") {
                tokens.push_back({TokenType::NUMBER, "", M_PI , start});
                expect_operand = false;
            } else if (word == "e") {
                tokens.push_back({TokenType::NUMBER, "", M_E, start});
                expect_operand = false;
            } else {
                tokens.push_back({TokenType::FUNCTION, word, 0, start});
                expect_operand = true;
            }
        } 
        else if (isdigit(expr[i]) || expr[i] == '.') {
            int start = i;
            while (i < n && (isdigit(expr[i]) || expr[i] == '.')) i++;
            double val = stod(expr.substr(start, i - start));
            tokens.push_back({TokenType::NUMBER, "", val, start});
            expect_operand = false;
        } 
        else if (expr[i] == '(') {
            tokens.push_back({TokenType::LPAREN, "(", 0, i});
            expect_operand = true;
            i++;
        } 
        else if (expr[i] == ')') {
            tokens.push_back({TokenType::RPAREN, ")", 0, i});
            expect_operand = false;
            i++;
        } 
        else if (expr[i] == ',') {
            tokens.push_back({TokenType::COMMA, ",", 0, i});
            expect_operand = true;
            i++;
        } 
        else {
            char op = expr[i];
            if (op == '+' || op == '-' || op == '*' || op == '/') {
                if (expect_operand && (op == '+' || op == '-')) {
                    // Unary operator handling
                    tokens.push_back({TokenType::OPERATOR, string(1, op == '-' ? '~' : '#'), 0, i});
                } else {
                    tokens.push_back({TokenType::OPERATOR, string(1, op), 0, i});
                }
                expect_operand = true;
                i++;
            } else {
                throw ParseError("Unknown character", i);
            }
        }
    }
    return tokens;
}

vector<Token> parse(const vector<Token>& tokens) {
    vector<Token> output;
    stack<Token> ops;

    for (const auto& t : tokens) {
        if (t.type == TokenType::NUMBER) {
            output.push_back(t);
        } else if (t.type == TokenType::FUNCTION) {
            ops.push(t);
        } else if (t.type == TokenType::COMMA) {
            bool found_paren = false;
            while (!ops.empty()) {
                if (ops.top().type == TokenType::LPAREN) {
                    found_paren = true;
                    break;
                }
                output.push_back(ops.top());
                ops.pop();
            }
            if (!found_paren) throw ParseError("Mismatched parenthesis or misplaced comma", t.pos);
        } else if (t.type == TokenType::OPERATOR) {
            while (!ops.empty() && ops.top().type == TokenType::OPERATOR) {
                string o1 = t.name;
                string o2 = ops.top().name;
                int p1 = precedence(o1);
                int p2 = precedence(o2);

                bool left_assoc = (o1 != "~" && o1 != "#"); 
                if (p2 > p1 || (p2 == p1 && left_assoc)) {
                    output.push_back(ops.top());
                    ops.pop();
                } else {
                    break;
                }
            }
            ops.push(t);
        } else if (t.type == TokenType::LPAREN) {
            ops.push(t);
        } else if (t.type == TokenType::RPAREN) {
            bool found = false;
            while (!ops.empty()) {
                if (ops.top().type == TokenType::LPAREN) {
                    found = true;
                    break;
                }
                output.push_back(ops.top());
                ops.pop();
            }
            if (!found) throw ParseError("Mismatched right parenthesis", t.pos);
            ops.pop(); // pop LPAREN

            if (!ops.empty() && ops.top().type == TokenType::FUNCTION) {
                output.push_back(ops.top());
                ops.pop();
            }
        }
    }
    while (!ops.empty()) {
        if (ops.top().type == TokenType::LPAREN || ops.top().type == TokenType::RPAREN) {
            throw ParseError("Mismatched opening parenthesis", ops.top().pos);
        }
        output.push_back(ops.top());
        ops.pop();
    }
    return output;
}

double evaluate(const vector<Token>& postfix) {
    stack<double> st;

    for (const auto& t : postfix) {
        if (t.type == TokenType::NUMBER) {
            st.push(t.value);
        } else if (t.type == TokenType::OPERATOR) {
            if (t.name == "~") {
                if (st.empty()) throw ParseError("Missing operand for unary minus", t.pos);
                double val = st.top(); st.pop();
                st.push(-val);
            } else if (t.name == "#") {
                if (st.empty()) throw ParseError("Missing operand for unary plus", t.pos);
                // unary plus effectively does nothing
            } else {
                if (st.size() < 2) throw ParseError("Missing operands for operator " + t.name, t.pos);
                double b = st.top(); st.pop();
                double a = st.top(); st.pop();
                if (t.name == "+") st.push(a + b);
                else if (t.name == "-") st.push(a - b);
                else if (t.name == "*") st.push(a * b);
                else if (t.name == "/") {
                    if (b == 0) throw ParseError("Division by zero", t.pos);
                    st.push(a / b);
                }
            }
        } else if (t.type == TokenType::FUNCTION) {
            if (t.name == "pow") {
                if (st.size() < 2) throw ParseError("pow requires 2 operands (e.g. pow(2,3))", t.pos);
                double b = st.top(); st.pop();
                double a = st.top(); st.pop();
                st.push(pow(a, b));
            } else {
                if (st.empty()) throw ParseError("Missing argument for function " + t.name, t.pos);
                double a = st.top(); st.pop();
                
                if (t.name == "sin") st.push(sin(a));
                else if (t.name == "cos") st.push(cos(a));
                else if (t.name == "tan") st.push(tan(a));
                else if (t.name == "log") st.push(log(a));
                else if (t.name == "log10") st.push(log10(a));
                else if (t.name == "exp") st.push(exp(a));
                else if (t.name == "sqrt") st.push(sqrt(a));
                else throw ParseError("Unknown or unsupported function: " + t.name, t.pos);
            }
        }
    }
    
    if (st.size() != 1) {
        throw ParseError("Invalid expression structure string", -1);
    }
    return st.top();
}

void executeCalculations() {
    string line;
    cout << "cppcalculator via Shunting-Yard (type 'exit' to quit)." << endl;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        if (line == "exit" || line == "quit") break;
        
        // Simple trim
        int s = 0; while (s < line.size() && isspace(line[s])) s++;
        if (s == line.size()) continue;

        auto start = chrono::high_resolution_clock::now();
        try {
            auto tokens = tokenize(line);
            auto postfix = parse(tokens);
            double result = evaluate(postfix);
            
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            
            cout << "ans = " << result << " (completed in " << duration.count() << " us)" << endl;
        } catch (const ParseError& e) {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            if (e.pos != -1) {
                cout << "ERROR at: " << e.pos << " (completed in " << duration.count() << " us)" << endl;
            } else {
                // If there's no precise position known (e.g. final stack wasn't length 1)
                cout << "ERROR at: " << line.length() << " [" << e.what() << "] (completed in " << duration.count() << " us)" << endl;
            }
        } catch (const exception& e) {
            cout << "FATAL ERROR: " << e.what() << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // If you want robust arguments passing:
    if (argc > 1) {
        string expr = "";
        for (int i = 1; i < argc; i++) {
            expr += argv[i];
            if (i < argc - 1) expr += " ";
        }
        
        auto start = chrono::high_resolution_clock::now();
        try {
            auto tokens = tokenize(expr);
            auto postfix = parse(tokens);
            double result = evaluate(postfix);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            
            cout << "ans = " << result << " (completed in " << duration.count() << " us)" << endl;
        } catch (const ParseError& e) {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            int loc = e.pos != -1 ? e.pos : expr.length();
            cout << "ERROR at: " << loc << " (completed in " << duration.count() << " us)" << endl;
        }
    } else {
        executeCalculations();
    }
    return 0;
}
