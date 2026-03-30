#include <iostream>
#include <tuple>
#include <array>
#include <string>

auto parseCanFrame(const std::string &raw_data) -> std::tuple<bool, int, std::array<double, 3>>;

auto main() -> int
{
    auto [is_valid, frame_id, accel] = parseCanFrame("ABCDEF");
    if (is_valid)
    {
        std::cout << "[Parse OK] ID: " << frame_id << ", Accel: " << accel[0] << ' ' << accel[1] << ' ' << accel[2] << std::endl;
    }
    else
    {
        std::cout << "[Parse Failed] Invalid frame." << std::endl;
    }

    std::tie(is_valid, frame_id, accel) = parseCanFrame("AB");
    if (is_valid)
    {
        std::cout << "[Parse OK] ID: " << frame_id << ", Accel: " << accel[0] << ' ' << accel[1] << ' ' << accel[2] << std::endl;
    }
    else
    {
        std::cout << "[Parse Failed] Invalid frame." << std::endl;
    }
}

auto parseCanFrame(const std::string &raw_data) -> std::tuple<bool, int, std::array<double, 3>>
{
    if (raw_data.length() > 5)
    {
        return {true, 0x123, {1.5, 2.5, 3.5}};
    }
    return {false, -1, {0, 0, 0}};
}
