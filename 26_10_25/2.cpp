#include <iostream>
#include <source_location>

#ifdef NDEBUG
#define trace() ((void) 0)
#else
#define trace() Tracer _func_tracer_guard
#endif

class Tracer {
private:
    std::source_location location;

public:
    explicit Tracer(std::source_location loc = std::source_location::current()) : location(loc) {
        std::cout << "[ENTER] Function: " << location.function_name() << " | File: " << location.file_name()
                  << " | Line: " << location.line() << std::endl;
    }

    ~Tracer() {
        std::cout << "[EXIT] Function: " << location.function_name() << " | Line: " << location.line() << std::endl;
    }

    Tracer(const Tracer &) = delete;
    Tracer &operator=(const Tracer &) = delete;
    Tracer(Tracer &&) = delete;
    Tracer &operator=(Tracer &&) = delete;
};

void calculate() {
    trace();

    std::cout << "Inside calculate" << std::endl;
}

int process(int value) {
    trace();

    if (value <  0) {
        std::cout << "Negative value" << std::endl;
        return 0;
    }

    calculate();

    return value - 1;
}

int main() {
    std::cout << "--- Start of Program ---" << std::endl;
    {
        trace();
        std::cout << "Calling processData(10)" << std::endl;
        process(10);
        std::cout << "Calling processData(-5)" << std::endl;
        process(-5);
    }
    std::cout << "--- End of Program ---" << std::endl;
    return 0;
}
