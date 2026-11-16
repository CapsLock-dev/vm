#include "core/logger.hpp"
#include "lexer/tokenizer.hpp"
#include <fstream>
#include <utility>

int main(int argc, char** argv) {
    core::logger::LogConfig cfg = {
        .min_level = core::logger::LogLevel::TRACE,
        .queue_size = 100,
        .colored_output = true,
    };
    core::logger::Logger::get_instance().init(cfg);

    std::string contents{};
    {
        std::fstream input(argv[1]);
        std::stringstream contents_stream;
        if (!input.is_open()) {
            LOG_ERROR("Can't open file");
            return 1;
        }
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
        input.close();
    }

    auto tokenizer = lexer::Tokenizer(std::move(contents));
    auto res = tokenizer.tokenize();

    core::logger::Logger::get_instance().shutdown();
    return 0;
}
