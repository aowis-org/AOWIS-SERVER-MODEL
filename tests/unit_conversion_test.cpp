#include <aowis/model/units/conversion.h>
#include <algorithm>
#include <cassert>
#include <cmath>

namespace u = aowis::units;

int main()
{
    const auto near = [](double actual, double expected) {
        return std::abs(actual - expected) <= 1e-12 * std::max(1.0, std::abs(expected));
    };
    assert(near(u::feetToMetres(1.0), 0.3048));
    assert(near(u::inchesToMillimetres(1.0), 25.4));
    assert(near(u::cubicFeetToCubicMetres(1.0), 0.028316846592));
    assert(near(u::usGallonsToCubicMetres(1.0), 0.003785411784));
    assert(near(u::millionUsGallonsPerDayToCubicMetresPerHour(1.0), 157.725491));
    assert(near(u::millionImperialGallonsPerDayToCubicMetresPerHour(1.0), 189.42041666666667));
    assert(near(u::cubicFeetPerSecondToCubicMetresPerHour(1.0), 101.9406477312));
    assert(near(u::usGallonsPerMinuteToCubicMetresPerHour(1.0), 0.22712470704));
    assert(near(u::litresPerSecondToCubicMetresPerHour(1.0), 3.6));
    assert(near(u::pascalsToMetresHead(9810.0, 1000.0, 9.81), 1.0));
    assert(!near(u::usSurveyFeetToMetres(1.0), u::feetToMetres(1.0)));
}
