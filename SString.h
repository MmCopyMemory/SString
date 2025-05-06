#pragma once
#include <array>
#include <string>

namespace _os_internal {
    constexpr uint32_t ct_hash(const char* str, uint32_t hash = 2166136261u) {
        return (*str == 0) ? hash : ct_hash(str + 1, (hash ^ static_cast<uint32_t>(*str)) * 16777619u);
    }
    constexpr uint32_t prng(uint32_t seed, int rounds = 6) {
        for (int i = 0; i < rounds; ++i)
            seed = (seed * 1664525u + 1013904223u);
        return seed;
    }
    template <std::size_t N, uint32_t Seed>
    struct KeyGen {
        std::array<uint8_t, N> keys{};
        constexpr KeyGen() {
            for (std::size_t i = 0; i < N; ++i)
                keys[i] = static_cast<uint8_t>(prng(Seed + static_cast<uint32_t>(i)) % 191 + 32);
        }
    };
    template <std::size_t N, uint32_t Seed>
    class obfuscated_string_t {
    private:
        std::array<char, N> encrypted{};
        static constexpr KeyGen<N, Seed> keygen{};
        constexpr static char encode(char c, std::size_t i) {
            uint8_t k = keygen.keys[i];
            if (i % 3 == 0)
                return static_cast<char>(c + (k ^ (i + 17)));
            else if (i % 3 == 1)
                return static_cast<char>(c + ((k * (i + 3)) % 127));
            else
                return static_cast<char>(c + k + static_cast<char>(i * i));
        }

        constexpr static char decode(char c, std::size_t i) {
            uint8_t k = keygen.keys[i];
            if (i % 3 == 0)
                return static_cast<char>(c - (k ^ (i + 17)));
            else if (i % 3 == 1)
                return static_cast<char>(c - ((k * (i + 3)) % 127));
            else
                return static_cast<char>(c - k - static_cast<char>(i * i));
        }

        constexpr obfuscated_string_t(const char(&str)[N]) {
            for (std::size_t i = 0; i < N; ++i)
                encrypted[i] = encode(str[i], i);
        }

    public:
        constexpr static obfuscated_string_t make(const char(&str)[N]) {
            return obfuscated_string_t(str);
        }

        std::string reveal() const {
            std::string out(N, '\0');
            for (std::size_t i = 0; i < N; ++i)
                out[i] = decode(encrypted[i], i);
            return out;
        }

        constexpr const char* raw() const { return encrypted.data(); }
    };

    template <std::size_t N, uint32_t S>
    constexpr KeyGen<N, S> obfuscated_string_t<N, S>::keygen;

}
#define OBFSTR(str) \
    (_os_internal::obfuscated_string_t<sizeof(str), _os_internal::ct_hash(__FILE__ __DATE__ __TIME__) + __LINE__ + __COUNTER__>::make(str))
