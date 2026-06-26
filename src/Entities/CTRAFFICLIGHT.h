#pragma once
#include <SFML/Graphics.hpp>

enum class LightState { Green, Blinking, Red };

class CTRAFFICLIGHT {
public:
    LightState state;
    float timer;
    float greenDuration;
    float redDuration;
    int frameIndex;
    float animTimer;
    sf::RectangleShape shape;

    CTRAFFICLIGHT() : state(LightState::Green), timer(0.f), greenDuration(3.f), redDuration(2.f), frameIndex(0), animTimer(0.f) {
        shape.setSize(sf::Vector2f(12.f, 30.f));
        shape.setFillColor(sf::Color::Green);
    }

    void update(float dt) {
        animTimer += dt;
        if (animTimer >= 0.15f) {
            animTimer = 0.f;
            frameIndex++;
        }
        
        timer -= dt;
        if (state == LightState::Red && timer <= 0.f) {
            state = LightState::Green;
        }

        if (state == LightState::Green) {
            shape.setFillColor(sf::Color::Green);
        } else {
            shape.setFillColor(sf::Color::Red);
        }
    }
};

