// tests/p3/test_p3.cpp — YOUR P3 TEST SUITE.
//
// The five tests below are worked examples. Keep them, and add your own
// until you have AT LEAST 15 (spec §6 lists what must be covered).
// Name every test <area>_<what>: reg_ for the registry, ws_ for the
// workspace, calc_ for the calculator.
//
// Build & run:   cmake --build build && ./build/test_p3
// One area:      ./build/test_p3 reg_        (names starting with "reg_")
// One test:      ./build/test_p3 =reg_add_and_find

#include "../mini_test.h"
#include "../test_helpers.h"     // TempDir, write_text, args1, ...

#include "tools/calculator.h"
#include "tools/tool_registry.h"
#include "tools/workspace.h"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;
int countTools = 0;

void countTool(const Tool&) {
    countTools++;
}

namespace {

    // Fill registry in tests
    class NamedTool : public Tool {
    public:
        explicit NamedTool(std::string n) : n_(std::move(n)), schema_(JsonValue::object()) {}
        std::string name() const override { return n_; }
        std::string description() const override { return "test tool"; }
        const JsonValue& schema() const override { return schema_; }
        ToolResult invoke(const JsonValue&, ToolContext&) override { return ToolResult::success(n_); }
    private:
        std::string n_;
        JsonValue   schema_;   
    };

    bool eval(const std::string& expr, double* value, std::string* error) {
        std::vector<CalcToken> tokens;
        if (!calc_tokenize(expr, &tokens, error)) return false;
        return calc_evaluate(tokens, value, error);
    }
}  

TEST(reg_add_and_find) {
    ToolRegistry r;
    auto t = std::make_unique<NamedTool>("alpha");
    Tool* raw = t.get();                       
    REQUIRE(r.add(std::move(t)));
    CHECK(r.size() == 1);
    CHECK(r.find("alpha") == raw);             
    CHECK(r.find("beta") == nullptr);
}

TEST(reg_duplicate_keeps_original) {
    ToolRegistry r;
    auto first = std::make_unique<NamedTool>("alpha");
    Tool* original = first.get();
    REQUIRE(r.add(std::move(first)));
    CHECK(!r.add(std::make_unique<NamedTool>("alpha")));
    CHECK(r.find("alpha") == original);
    CHECK(r.size() == 1);
}

TEST(ws_rejects_parent_escape) {
    TempDir d;
    Workspace ws(d.path());
    fs::path out;
    std::string reason;
    REQUIRE(ws.resolve("notes/todo.txt", &out, &reason));  
    CHECK(out == ws.root() / "notes" / "todo.txt");
    CHECK(!ws.resolve("../etc/passwd", &out, &reason));
    CHECK(!reason.empty());
}

TEST(calc_precedence) {
    double v = 0;
    std::string err;
    REQUIRE(eval("2 + 3 * 4", &v, &err));
    CHECK(v == 14);
}

TEST(calc_division_by_zero) {
    double v = 0;
    std::string err;
    CHECK(!eval("1 / 0", &v, &err));
    CHECK(err == "division by zero");
}

// ------------------------------------------------------------- your tests

TEST(reg_empty) {
    ToolRegistry r;

    CHECK(r.size() == 0);
    CHECK(r.bucket_count() >= 1);
    CHECK(r.bucket_count() <= 64);
}

TEST(reg_null_rejected) {
    ToolRegistry r;

    CHECK(!r.add(nullptr));
    CHECK(r.size() == 0);
}

TEST(reg_missing_tool) {
    ToolRegistry r;

    CHECK(r.find("hello") == nullptr);
}

TEST(reg_multiple_tools) {
    ToolRegistry r;

    r.add(std::make_unique<NamedTool>("one"));
    r.add(std::make_unique<NamedTool>("two"));
    r.add(std::make_unique<NamedTool>("three"));

    CHECK(r.size() == 3);
    CHECK(r.find("one") != nullptr);
    CHECK(r.find("two") != nullptr);
    CHECK(r.find("three") != nullptr);
}

TEST(reg_10000_tools) {
    ToolRegistry r;

    for (int i = 0; i < 10000; i++) {
        std::string name = "tool" + std::to_string(i);

        REQUIRE(r.add(std::make_unique<NamedTool>(name)));
    }

    CHECK(r.size() == 10000);
    CHECK(r.load_factor() <= 1.0);

    for (int i = 0; i < 10000; i++) {
        std::string name = "tool" + std::to_string(i);

        CHECK(r.find(name) != nullptr);
    }

    std::printf("Buckets: %zu\n", r.bucket_count());
    std::printf("Load factor: %f\n", r.load_factor());
    std::printf("Longest chain: %zu\n", r.max_probe_length());
}

TEST(ws_empty_path) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(!ws.resolve("", &out, &reason));
}

TEST(ws_absolute_path) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(!ws.resolve("/etc/passwd", &out, &reason));
}

TEST(ws_normal_path) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(ws.resolve("notes/file.txt", &out, &reason));
}

TEST(calc_parentheses) {
    double value = 0;
    std::string error;

    REQUIRE(eval("(2 + 3) * 4", &value, &error));

    CHECK(value == 20);
}

TEST(calc_subtraction) {
    double value = 0;
    std::string error;

    REQUIRE(eval("10 - 4 - 3", &value, &error));

    CHECK(value == 3);
}
TEST(reg_for_each) {
    ToolRegistry r;

    r.add(std::make_unique<NamedTool>("one"));
    r.add(std::make_unique<NamedTool>("two"));
    r.add(std::make_unique<NamedTool>("three"));

    countTools = 0;

    r.for_each(countTool);

    CHECK(countTools == 3);
}
TEST(ws_parent_path) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(!ws.resolve("a/../../b", &out, &reason));
}

TEST(ws_dot_path) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(ws.resolve(".", &out, &reason));
    CHECK(out == ws.root());
}

TEST(ws_normalized_path) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(ws.resolve("a/../b", &out, &reason));
    CHECK(out == ws.root() / "b");
}

TEST(calc_division) {
    double value = 0;
    std::string error;

    REQUIRE(eval("100 / 10 / 5", &value, &error));

    CHECK(value == 2);
}

TEST(calc_mixed_operations) {
    double value = 0;
    std::string error;

    REQUIRE(eval("2 * (3 + 4) - 5", &value, &error));

    CHECK(value == 9);
}

TEST(calc_negative_multiplication) {
    double value = 0;
    std::string error;

    REQUIRE(eval("-3 * -3", &value, &error));

    CHECK(value == 9);
}

TEST(calc_decimal_result) {
    double value = 0;
    std::string error;

    REQUIRE(eval("7 / 2", &value, &error));

    CHECK(value == 3.5);
}

TEST(calc_parentheses_division_zero) {
    double value = 0;
    std::string error;

    CHECK(!eval("1 / (2 - 2)", &value, &error));

    CHECK(error == "division by zero");
}

TEST(ws_double_slash) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(ws.resolve("./notes//todo.txt", &out, &reason));
    CHECK(out == ws.root() / "notes" / "todo.txt");
}

TEST(ws_back_to_root) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(ws.resolve("a/..", &out, &reason));
    CHECK(out == ws.root());
}

TEST(ws_new_directory) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(ws.resolve("new/dir/file.txt", &out, &reason));
    CHECK(out == ws.root() / "new" / "dir" / "file.txt");
}

TEST(ws_dots_in_name) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(ws.resolve("my..notes.txt", &out, &reason));
    CHECK(out == ws.root() / "my..notes.txt");
}

TEST(ws_parent_only) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(!ws.resolve("..", &out, &reason));
}

TEST(ws_escape_and_return) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    CHECK(!ws.resolve("notes/../../ws/notes/todo.txt", &out, &reason));
}

TEST(ws_null_character) {
    TempDir d;
    Workspace ws(d.path());

    fs::path out;
    std::string reason;

    std::string path = "notes";
    path += '\0';
    path += "file.txt";

    CHECK(!ws.resolve(path, &out, &reason));
}

TEST(reg_alternative_hash) {
    ToolRegistry r;

    for (int i = 0; i < 10000; i++) {
        std::string name = "tool_" + std::to_string(i);
        REQUIRE(r.add(std::make_unique<NamedTool>(name)));
    }
    std::vector<std::size_t> buckets(r.bucket_count(), 0);

    for (int i = 0; i < 10000; i++) {
        std::string name = "tool_" + std::to_string(i);

        int hash = name[0] + name.size();
        int index = hash % buckets.size();

        buckets[index]++;
    }

    std::size_t longest = 0;

    for (std::size_t i = 0; i < buckets.size(); i++) {
        if (buckets[i] > longest) {
            longest = buckets[i];
        }
    }

    std::printf("Our hash longest chain: %zu\n", r.max_probe_length());
    std::printf("Weak hash longest chain: %zu\n", longest);

    CHECK(r.max_probe_length() <= 64);
    CHECK(longest > r.max_probe_length());
}

int main(int argc, char** argv) { return mt::run_all(argc, argv); }
