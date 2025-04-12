#pragma once

#include "SFML/Audio.hpp"
#include <cmath>

namespace ui_sound {

inline void tone(const std::vector<i64> &freqList) {
    std::vector<sf::Int16> samples;

    short value = 1;
    for (i64 freq : freqList) {
        for (i64 i = 0; i < freq / 6; i++) {
            samples.push_back(32767 * 1 * value);
        }
        value = !value;
    }

    sf::SoundBuffer _sndBuffer;
    _sndBuffer.loadFromSamples(&samples[0], samples.size(), 1, 44100);

    sf::Sound sound;

    sound.setBuffer(_sndBuffer);
    sound.setVolume(10);
    sound.play();

    while (sound.getStatus() == sf::Sound::Playing)
        ;

    sound.resetBuffer();
}

} // namespace ui_sound
