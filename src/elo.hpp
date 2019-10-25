#ifndef ELO_HPP
#define ELO_HPP

#include <cassert>
#include <climits>
#include <cmath>

class Elo {
   public:
    constexpr Elo(const int wins, const int losses, const int draws)
        : wins_{wins}, losses_{losses}, draws_{draws} {
        if (wins < 0 || losses < 0 || draws < 0) {
            throw "asd";
        }
    }

    [[nodiscard]] constexpr float elo() const noexcept {
        const auto total = wins_ + losses_ + draws_;

        if (total == 0) {
            return std::nanf("");
        }

        const float m_mu = (wins_ + static_cast<float>(draws_) / 2.0) / total;

        const auto x = diff(m_mu);
        if (x == -0.0) {
            return 0.0;
        }
        return x;
    }

    [[nodiscard]] constexpr float err() const noexcept {
        const auto total = wins_ + losses_ + draws_;

        if (total == 0) {
            return std::nanf("");
        }

        const float m_mu = (wins_ + static_cast<float>(draws_) / 2.0) / total;

        const float w = static_cast<float>(wins_) / total;
        const float l = static_cast<float>(losses_) / total;
        const float d = static_cast<float>(draws_) / total;

        const float devW = w * std::pow(1.0 - m_mu, 2.0);
        const float devL = l * std::pow(0.0 - m_mu, 2.0);
        const float devD = d * std::pow(0.5 - m_mu, 2.0);

        const float m_stdev = std::sqrt(devW + devL + devD) / std::sqrt(total);

        const float muMin = m_mu + phiInv(0.025) * m_stdev;
        const float muMax = m_mu + phiInv(0.975) * m_stdev;
        return (diff(muMax) - diff(muMin)) / 2.0;
    }

    [[nodiscard]] constexpr float los() const noexcept {
        if (wins_ + losses_ == 0) {
            return std::nanf("");
        }
        return 100 * (0.5 + 0.5 * std::erf((wins_ - losses_) /
                                           std::sqrt(2.0 * (wins_ + losses_))));
    }

    [[nodiscard]] constexpr float drawRatio() const noexcept {
        const auto total = wins_ + losses_ + draws_;

        if (total == 0) {
            return std::nanf("");
        }

        return static_cast<float>(draws_) / total;
    }

   private:
    [[nodiscard]] constexpr float erfInv(const float x) const noexcept {
        assert(x < 1.0);
        assert(x > -1.0);
        const auto a = 8.0 * (M_PI - 3.0) / (3.0 * M_PI * (4.0 - M_PI));
        const auto y = std::log10(1.0 - x * x);
        const auto z = 2.0 / (M_PI * a) + y / 2.0;
        const auto ret = std::sqrt(std::sqrt(z * z - y / a) - z);

        if (x >= 0.0) {
            return ret;
        } else {
            return -ret;
        }
    }

    [[nodiscard]] constexpr float phiInv(const float p) const noexcept {
        return std::sqrt(2.0) * erfInv(2.0 * p - 1.0);
    }

    [[nodiscard]] constexpr float diff(const float p) const noexcept {
        if (p >= 1.0) {
            return std::numeric_limits<float>::infinity();
        } else if (p <= 0.0) {
            return -std::numeric_limits<float>::infinity();
        }
        return -400.0 * std::log10(1.0 / p - 1.0);
    }

   private:
    int wins_;
    int losses_;
    int draws_;
};

#endif
