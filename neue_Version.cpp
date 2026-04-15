 
#include <iostream>
#include <array>
#include <bitset>
#include <chrono>
#include <thread>
#include <ctime>

class BinaryClock {
public:
    void run() const {
        while (true) {
            auto now = getCurrentTime();
            auto digits = splitDigits(now);
            auto binary = convertToBinary(digits);

            clearScreen();
            render(binary);

            sleepUntilNextSecond();
        }
    }

private:
    struct TimeDigits {
        int h_tens, h_ones;
        int m_tens, m_ones;
        int s_tens, s_ones;
    };

    static std::tm getCurrentTime() {
        std::time_t t = std::time(nullptr);
        std::tm tm{};

#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        return tm;
    }

    static TimeDigits splitDigits(const std::tm& tm) {
        return {
            tm.tm_hour / 10, tm.tm_hour % 10,
            tm.tm_min  / 10, tm.tm_min  % 10,
            tm.tm_sec  / 10, tm.tm_sec  % 10
        };
    }

    using BinaryDigits = std::array<std::bitset<4>, 6>;

    static BinaryDigits convertToBinary(const TimeDigits& d) {
        return {
            std::bitset<4>(d.h_tens),
            std::bitset<4>(d.h_ones),
            std::bitset<4>(d.m_tens),
            std::bitset<4>(d.m_ones),
            std::bitset<4>(d.s_tens),
            std::bitset<4>(d.s_ones)
        };
    }

    static void render(const BinaryDigits& digits) {
        constexpr int ROWS = 4;
        constexpr int COLS = 6;

        std::cout << "Binary Clock (HH:MM:SS)\n\n";

        for (int row = ROWS - 1; row >= 0; --row) {
            for (int col = 0; col < COLS; ++col) {
                std::cout << (digits[col][row] ? "● " : "○ ");
            }
            std::cout << '\n';
        }

        std::cout << "\n";
    }

    static void sleepUntilNextSecond() {
        using namespace std::chrono;

        auto now = system_clock::now();
        auto next = time_point_cast<seconds>(now) + seconds(1);

        std::this_thread::sleep_until(next);
    }

    static void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
};

int main() {
    BinaryClock clock;
    clock.run();
    return 0;
}
