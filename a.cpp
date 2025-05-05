#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 600), "Super Mario - C++ SFML");
    window.setFramerateLimit(60);

    // Mario Texture
    sf::Texture marioTexture;
    if (!marioTexture.loadFromFile("mario.png"))
        return -1;

    sf::Sprite mario(marioTexture);
    mario.setScale(0.07f, 0.13f);  // تقريبًا 16x32
    mario.setPosition(50.f, 500.f);

    // Font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return -1;

    sf::Text message;
    message.setFont(font);
    message.setCharacterSize(40);
    message.setFillColor(sf::Color::White);
    message.setPosition(300, 250);
    bool gameOver = false, gameWin = false;

    // Restart Text
    sf::Text restartText;
    restartText.setFont(font);
    restartText.setCharacterSize(30);
    restartText.setString("Press R to Restart");
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(330, 320);

    // Physics
    sf::Vector2f velocity(0.f, 0.f);
    const float gravity = 0.5f;
    const float jumpStrength = -12.f;
    bool isOnGround = true;

    // Ground (Green Earth)
    sf::RectangleShape ground(sf::Vector2f(2000.f, 100.f));
    ground.setPosition(0, 550);
    ground.setFillColor(sf::Color::Green);  // Change to green for earth

    // Obstacles
    std::vector<sf::RectangleShape> obstacles;
    std::srand(static_cast<unsigned>(time(0)));

    float xPos = 200.f;
    while (xPos < 1800.f) { // تخلي الحواجز تنتهي قبل خط النهاية
        float redWidth = 15.f;  // Narrower red blocks
        float redHeight = 40.f;
        float blackWidth = redWidth / 2.f;
        float blackHeight = 60.f;  // Taller black blocks

        bool isBlack = (std::rand() % 6 == 0);

        sf::RectangleShape obs;
        if (isBlack) {
            obs.setSize(sf::Vector2f(blackHeight, blackWidth));
            obs.setFillColor(sf::Color::Black);
        } else {
            obs.setSize(sf::Vector2f(redHeight, redWidth));
            obs.setFillColor(sf::Color::Red);
        }

        // Randomize obstacle height, ensuring no block is below ground level
        // Avoid positioning too close to ground
        float yLevel = 350.f + rand() % 200;  // Raise obstacles up from ground level
        obs.setPosition(xPos, yLevel);
        obstacles.push_back(obs);

        xPos += static_cast<float>(30 + rand() % 50);
    }

    // Coins
    std::vector<sf::CircleShape> coins;
    int totalCoins = 10;
    int collectedCoins = 0;
    for (int i = 0; i < totalCoins; ++i) {
        sf::CircleShape coin(10.f);
        coin.setFillColor(sf::Color::Yellow);

        // Coins placement ensures they are above ground and not too high
        float x = 250.f + rand() % 1600;
        float y = 400.f + rand() % 100;  // Ensure coins are not below the ground level (adjust y accordingly)
        coin.setPosition(x, y);
        coins.push_back(coin);
    }

    // Progress Bar Background
    sf::RectangleShape barBack(sf::Vector2f(200.f, 20.f));
    barBack.setFillColor(sf::Color(100, 100, 100));
    barBack.setPosition(20.f, 20.f);

    // Progress Bar Fill
    sf::RectangleShape barFill(sf::Vector2f(0.f, 20.f));
    barFill.setFillColor(sf::Color::Green);
    barFill.setPosition(20.f, 20.f);

    // Finish Line
    sf::Texture finishTexture;
    sf::Sprite finishSprite;
    if (!finishTexture.loadFromFile("finish.png")) return -1;
    finishSprite.setTexture(finishTexture);
    finishSprite.setScale(0.15f, 0.3f);
    finishSprite.setPosition(1900.f, 400.f);

    // Sun (Yellow Circle)
    sf::CircleShape sun(50.f);  // Radius of 50 for the sun
    sun.setFillColor(sf::Color::Yellow);
    sun.setPosition(900.f, 50.f);  // Positioning the sun

    // View (Camera)
    sf::View view = window.getDefaultView();

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if ((gameOver || gameWin) && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                mario.setPosition(50.f, 500.f);
                velocity = {0.f, 0.f};
                gameOver = gameWin = false;
                collectedCoins = 0;
                barFill.setSize(sf::Vector2f(0.f, 20.f));
                view.setCenter(500.f, 300.f);
            }
        }

        if (!gameOver && !gameWin) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                mario.move(-2.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                mario.move(2.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isOnGround) {
                velocity.y = jumpStrength;
                isOnGround = false;
            }

            velocity.y += gravity;
            mario.move(0, velocity.y);

            if (mario.getPosition().y + mario.getGlobalBounds().height >= ground.getPosition().y) {
                mario.setPosition(mario.getPosition().x, ground.getPosition().y - mario.getGlobalBounds().height);
                velocity.y = 0.f;
                isOnGround = true;
            }

            for (auto& obs : obstacles) {
                if (mario.getGlobalBounds().intersects(obs.getGlobalBounds())) {
                    sf::FloatRect marioBounds = mario.getGlobalBounds();
                    sf::FloatRect obsBounds = obs.getGlobalBounds();

                    bool isBlack = (obs.getFillColor() == sf::Color::Black);

                    float deltaRight = (marioBounds.left + marioBounds.width) - obsBounds.left;
                    float deltaLeft = (obsBounds.left + obsBounds.width) - marioBounds.left;
                    float deltaBottom = (marioBounds.top + marioBounds.height) - obsBounds.top;
                    float deltaTop = (obsBounds.top + obsBounds.height) - marioBounds.top;

                    float overlapX = std::min(deltaRight, deltaLeft);
                    float overlapY = std::min(deltaBottom, deltaTop);

                    if (isBlack) {
                        message.setString("Game Over!");
                        gameOver = true;
                    } else {
                        if (overlapX < overlapY) {
                            if (deltaRight < deltaLeft)
                                mario.move(-overlapX, 0);
                            else
                                mario.move(overlapX, 0);
                        } else {
                            if (velocity.y > 0 && deltaBottom < deltaTop) {
                                mario.setPosition(mario.getPosition().x, obsBounds.top - marioBounds.height);
                                velocity.y = 0.f;
                                isOnGround = true;
                            } else if (velocity.y < 0 && deltaTop < deltaBottom) {
                                mario.setPosition(mario.getPosition().x, obsBounds.top + obsBounds.height + 1.f);
                                velocity.y = 0.f;
                            }
                        }
                    }
                }
            }

            // Coin collision
            for (auto it = coins.begin(); it != coins.end();) {
                if (mario.getGlobalBounds().intersects(it->getGlobalBounds())) {
                    it = coins.erase(it);
                    collectedCoins++;
                    float progress = (float)collectedCoins / totalCoins;
                    barFill.setSize(sf::Vector2f(200.f * progress, 20.f));
                } else {
                    ++it;
                }
            }

            if (mario.getGlobalBounds().intersects(finishSprite.getGlobalBounds())) {
                message.setString("You Win!");
                gameWin = true;
            }

            float camX = mario.getPosition().x;
            if (camX < 400.f) camX = 400.f;
            if (camX > 1600.f) camX = 1600.f;
            view.setCenter(camX, 300.f);
        }

        // Moving the sun with the camera view
        sun.setPosition(view.getCenter().x - 400.f + 900.f, 50.f);  // Adjust sun's position

        window.clear(sf::Color::Blue);  // Sky blue
        window.setView(view);
        window.draw(ground);
        for (auto& obs : obstacles)
            window.draw(obs);
        for (auto& coin : coins)
            window.draw(coin);
        window.draw(finishSprite);
        window.draw(mario);
        window.draw(sun);  // Draw the sun

        // UI elements
        window.setView(window.getDefaultView());
        window.draw(barBack);
        window.draw(barFill);

        if (gameOver || gameWin) {
            window.draw(message);
            window.draw(restartText);
        }

        window.display();
    }

    return 0;
}
