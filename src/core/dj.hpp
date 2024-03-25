#pragma once

#include <SFML/Audio.hpp>
#include <cmath>

#define TWOPI 6.283185307

class Dj {
public:
    void tone(double freq, int duration) {
        for (int i = 0; i < duration; i++) {
            _samples.push_back(_squareWave(i, freq, 0.7));
        }

        _sndBuffer.loadFromSamples(&_samples[0], _samples.size(), 1, 44100);
        _sound.setBuffer(_sndBuffer);
        _sound.setVolume(10);
        _sound.play();

        while (_sound.getStatus() == _sound.Playing)
            ;

        _samples.clear();
        _sound.resetBuffer();
    }

private:
    sf::SoundBuffer _sndBuffer;
    std::vector<sf::Int16> _samples;
    sf::Sound _sound;

    short _squareWave(double time, double freq, double amp) {
        if (freq == 0)
            return 0;

        short result = 0;
        int tpc = 44100 / freq;
        int cyclepart = int(time) % tpc;
        int halfcycle = tpc / 2;
        short amplitude = 32767 * amp;

        if (cyclepart < halfcycle) {
            result = amplitude;
        }

        return result;
    }

    short _sineWave(double time, double freq, double amp) {
        short result;

        double tpc = 44100 / freq;
        double cycles = time / tpc;
        double rad = TWOPI * cycles;
        short amplitude = 32767 * amp;
        result = amplitude * sin(rad);

        return result;
    }
};