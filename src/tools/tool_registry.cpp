// src/tools/tool_registry.cpp — YOU IMPLEMENT (spec §4.3).
//
// A hand-written hash table. No std::unordered_map / std::map / std::set.
// Every function below is a stub so the starter compiles. Replace each body.

#include "tools/tool_registry.h"

ToolRegistry::ToolRegistry() {
    buckets.resize(8);
}
ToolRegistry::~ToolRegistry() = default;   // unique_ptrs in your buckets free the tools


bool ToolRegistry::add(std::unique_ptr<Tool> tool) {

    if (tool == nullptr) {
        return false;
    }

    if (find(tool->name()) != nullptr) {
        return false;
    }

    // Resize if adding another tool makes the table too full
    if (double(toolCount + 1) / buckets.size() > 0.75) {
        std::vector<std::vector<std::unique_ptr<Tool>>> newBuckets;
        newBuckets.resize(buckets.size() * 2);

        for (auto& bucket : buckets) {
            for (auto& oldTool : bucket) {
                std::size_t index = hash(oldTool->name()) % newBuckets.size();
                newBuckets[index].push_back(std::move(oldTool));
            }
        }
        buckets = std::move(newBuckets);
    }
    std::size_t index = hash(tool->name()) % buckets.size();
    buckets[index].push_back(std::move(tool));
    toolCount++;

    return true;
}
Tool* ToolRegistry::find(std::string_view name) const {
    std::size_t index = hash(name) % buckets.size();

    for (const auto& tool : buckets[index]) {

        if (tool->name() == name) {
            return tool.get();
        }
    }
    return nullptr;
}

std::size_t ToolRegistry::hash(std::string_view key) noexcept {

    std::size_t number = 1469598103934665603ULL;
    for (unsigned char c : key) {
        number = number ^ c;
        number = number * 1099511628211ULL;
    }
    return number;
}


std::size_t ToolRegistry::size() const noexcept {
     return toolCount;
}

void ToolRegistry::for_each(const std::function<void(const Tool&)>& fn) const {

    for (const auto& bucket : buckets) {

        for (const auto& tool : bucket) {
            fn(*tool);
        }
    }
}

std::size_t ToolRegistry::bucket_count() const noexcept {
    return buckets.size();
}

double ToolRegistry::load_factor() const noexcept {
    return double(toolCount) / buckets.size();
}

std::size_t ToolRegistry::max_probe_length() const {
    std::size_t longest = 0;

    for (const auto& bucket : buckets) {

        if (bucket.size() > longest) {
            longest = bucket.size();
        }
    }
    return longest;
}
