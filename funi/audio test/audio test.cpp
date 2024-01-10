#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>

float dotProduct(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

bool isIntersecting(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4) {
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p2.x - p1.x;
    s1_y = p2.y - p1.y;
    s2_x = p4.x - p3.x;
    s2_y = p4.y - p3.y;

    float s, t;
    s = (-s1_y * (p1.x - p3.x) + s1_x * (p1.y - p3.y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = (s2_x * (p1.y - p3.y) - s2_y * (p1.x - p3.x)) / (-s2_x * s1_y + s1_x * s2_y);

    return s >= 0 && s <= 1 && t >= 0 && t <= 1;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML Window");
    window.setFramerateLimit(1000);

    sf::VertexArray line(sf::Lines, 8);
    line[0].position = sf::Vector2f(100.0f, 100.0f);
    line[1].position = sf::Vector2f(1800.0f, 100.0f);
    line[2].position = sf::Vector2f(100.0f, 900.0f);
    line[3].position = sf::Vector2f(100.0f, 100.0f);
    line[4].position = sf::Vector2f(100.0f, 900.0f);
    line[5].position = sf::Vector2f(1800.0f, 900.0f);
    line[6].position = sf::Vector2f(1800.0f, 900.0f);
    line[7].position = sf::Vector2f(1800.0f, 100.0f);

    for (int i = 0; i < 8; ++i) {
        line[i].color = sf::Color::White;
    }

    sf::RectangleShape blueSquare(sf::Vector2f(20.0f, 20.0f));
    blueSquare.setPosition(100.0f, 200.0f);
    blueSquare.setFillColor(sf::Color::Blue);

    sf::RectangleShape redSquare(sf::Vector2f(20.0f, 20.0f));
    redSquare.setPosition(300.0f, 200.0f);
    redSquare.setFillColor(sf::Color::Red);

    sf::Vector2f blueVelocity(300.0f, 30.0f);
    sf::Vector2f redVelocity(-300.0f, -300.0f);

    float speedMultiplier = 5.0f; // You can modify this value to control the speed

    sf::Clock clock;

    // Load collision sound
    sf::SoundBuffer collisionBuffer;
    if (!collisionBuffer.loadFromFile("sample.wav")) {
        // Print an error message to the console
        return -1; // Exit the program if sound file loading fails
    }

    sf::Sound collisionSound;
    collisionSound.setBuffer(collisionBuffer);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        // Handle keyboard input for blue square
        sf::Vector2f newBlueVelocity(0.0f, 0.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            newBlueVelocity.x = -300.0f * speedMultiplier;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            newBlueVelocity.x = 300.0f * speedMultiplier;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            newBlueVelocity.y = -300.0f * speedMultiplier;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            newBlueVelocity.y = 300.0f * speedMultiplier;
        }

        // Update blue velocity only if a new arrow key is pressed
        if (newBlueVelocity != sf::Vector2f(0.0f, 0.0f)) {
            blueVelocity = newBlueVelocity;
        }

        // Move the blue square based on velocity
        blueSquare.move(blueVelocity * dt);

        // Check for collision with the window boundaries for blue square
        if (blueSquare.getPosition().x < 0 || blueSquare.getPosition().x + blueSquare.getSize().x > window.getSize().x) {
            blueVelocity.x = -blueVelocity.x; // Bounce off the horizontal walls
        }

        if (blueSquare.getPosition().y < 0 || blueSquare.getPosition().y + blueSquare.getSize().y > window.getSize().y) {
            blueVelocity.y = -blueVelocity.y; // Bounce off the vertical walls
        }

        // Check for collision with the white lines for blue square
        for (int i = 0; i < 7; i += 2) {
            sf::Vector2f p1 = line[i].position;
            sf::Vector2f p2 = line[i + 1].position;

            if (isIntersecting(p1, p2, blueSquare.getPosition(), blueSquare.getPosition() + sf::Vector2f(blueSquare.getSize().x, 0.0f)) ||
                isIntersecting(p1, p2, blueSquare.getPosition(), blueSquare.getPosition() + sf::Vector2f(0.0f, blueSquare.getSize().y)) ||
                isIntersecting(p1, p2, blueSquare.getPosition() + sf::Vector2f(blueSquare.getSize().x, 0.0f), blueSquare.getPosition() + blueSquare.getSize()) ||
                isIntersecting(p1, p2, blueSquare.getPosition() + sf::Vector2f(0.0f, blueSquare.getSize().y), blueSquare.getPosition() + blueSquare.getSize())) {

                // Calculate penetration depth
                sf::Vector2f normal = sf::Vector2f(p2.y - p1.y, p1.x - p2.x); // Normal to the line
                float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
                normal /= length;

                // Calculate new velocity using reflection formula
                float dot = dotProduct(blueVelocity, normal);
                blueVelocity -= 2.0f * dot * normal;

                // Move the blue square slightly away from the collision point to prevent immediate re-collision
                blueSquare.move(blueVelocity * dt);

                // Increase the size of the blue square
                blueSquare.setSize(sf::Vector2f(blueSquare.getSize().x * 1.0f, blueSquare.getSize().y * 1.0f));

                // Play collision sound
                collisionSound.play();
            }
        }

        // Move the blue square based on velocity
        blueSquare.move(blueVelocity * dt);

        // Handle keyboard input for red square
        sf::Vector2f newRedVelocity(0.0f, 0.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            newRedVelocity.x = -300.0f * speedMultiplier;  // Move left
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            newRedVelocity.x = 300.0f * speedMultiplier;   // Move right
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            newRedVelocity.y = -300.0f * speedMultiplier;  // Move up
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            newRedVelocity.y = 300.0f * speedMultiplier;   // Move down
        }

        // Update red velocity only if a new arrow key is pressed
        if (newRedVelocity != sf::Vector2f(0.0f, 0.0f)) {
            redVelocity = newRedVelocity;
        }

        // Move the red square based on velocity
        redSquare.move(redVelocity * dt);

        // Check for collision with the window boundaries for red square
        if (redSquare.getPosition().x < 0 || redSquare.getPosition().x + redSquare.getSize().x > window.getSize().x) {
            redVelocity.x = -redVelocity.x; // Bounce off the horizontal walls
        }

        if (redSquare.getPosition().y < 0 || redSquare.getPosition().y + redSquare.getSize().y > window.getSize().y) {
            redVelocity.y = -redVelocity.y; // Bounce off the vertical walls
        }

        // Check for collision with the white lines for red square
        for (int i = 0; i < 7; i += 2) {
            sf::Vector2f p1 = line[i].position;
            sf::Vector2f p2 = line[i + 1].position;

            if (isIntersecting(p1, p2, redSquare.getPosition(), redSquare.getPosition() + sf::Vector2f(redSquare.getSize().x, 0.0f)) ||
                isIntersecting(p1, p2, redSquare.getPosition(), redSquare.getPosition() + sf::Vector2f(0.0f, redSquare.getSize().y)) ||
                isIntersecting(p1, p2, redSquare.getPosition() + sf::Vector2f(redSquare.getSize().x, 0.0f), redSquare.getPosition() + redSquare.getSize()) ||
                isIntersecting(p1, p2, redSquare.getPosition() + sf::Vector2f(0.0f, redSquare.getSize().y), redSquare.getPosition() + redSquare.getSize())) {

                // Calculate penetration depth
                sf::Vector2f normal = sf::Vector2f(p2.y - p1.y, p1.x - p2.x); // Normal to the line
                float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
                normal /= length;

                // Calculate new velocity using reflection formula
                float dot = dotProduct(redVelocity, normal);
                redVelocity -= 2.0f * dot * normal;

                // Move the red square slightly away from the collision point to prevent immediate re-collision
                redSquare.move(redVelocity * dt);

                // Increase the size of the red square
                redSquare.setSize(sf::Vector2f(redSquare.getSize().x * 1.0f, redSquare.getSize().y * 1.0f));

                // Play collision sound
                collisionSound.play();
            }
        }

        // Move the red square based on velocity
        redSquare.move(redVelocity * dt);

        // Check for collision between blue and red squares
        if (blueSquare.getGlobalBounds().intersects(redSquare.getGlobalBounds())) {
            // Swap velocities to simulate bouncing off each other
            sf::Vector2f tempVelocity = blueVelocity;
            blueVelocity = redVelocity;
            redVelocity = tempVelocity;

            // Move the squares slightly away from the collision point to prevent immediate re-collision
            blueSquare.move(blueVelocity * dt);
            redSquare.move(redVelocity * dt);

            // Increase the size of both squares
            blueSquare.setSize(sf::Vector2f(blueSquare.getSize().x * 1.000f, blueSquare.getSize().y * 1.000f));
            redSquare.setSize(sf::Vector2f(redSquare.getSize().x * 1.000f, redSquare.getSize().y * 1.00f));

            // Play collision sound
            collisionSound.play();
        }

        window.clear();
        window.draw(line);
        window.draw(blueSquare);
        window.draw(redSquare);
        window.display();
    }

    return 0;
}
