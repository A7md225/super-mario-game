#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>  // إضافة مكتبة الصوت
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iostream>

// Enemy Struct 
struct Enemy {
    sf::Sprite sprite;
    float speed;
    float minX, maxX;
    bool movingRight;
    bool defeated = false;
    sf::Clock defeatClock;

    Enemy(sf::Texture& texture, float x, float y, float moveSpeed, float rangeMin, float rangeMax) {
        sprite.setTexture(texture);
        sprite.setScale(0.0273f, 0.0507f); // حجم العدو زي ماريو
        sprite.setPosition(x, y);
        speed = moveSpeed;
        minX = rangeMin;
        maxX = rangeMax;
        movingRight = true;
    }

    void update() {
        if (defeated) {
            if (defeatClock.getElapsedTime().asSeconds() > 0.5f)
                sprite.setPosition(-100, -100); // اختفاء بعد شوية
            return;
        }

        if (movingRight) {
            sprite.move(speed, 0);
            if (sprite.getPosition().x >= maxX)
                movingRight = false;
        } else {
            sprite.move(-speed, 0);
            if (sprite.getPosition().x <= minX)
                movingRight = true;
        }
    }

    void defeat() {
        if (!defeated) {
            defeated = true;
            sprite.setRotation(180); // العدو اتقلب
            defeatClock.restart();
        }
    }

    sf::FloatRect getGlobalBounds() {
        return sprite.getGlobalBounds();
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 600), "Super Mario - C++ SFML");
    window.setFramerateLimit(60);

    // تحميل الأصوات - استخدام WAV للمؤثرات الصوتية
    sf::SoundBuffer jumpBuffer, coinBuffer, enemyDefeatBuffer, gameOverBuffer, winBuffer;
    if (!jumpBuffer.loadFromFile("jump.wav") ||
        !coinBuffer.loadFromFile("coin.wav") ||
        !enemyDefeatBuffer.loadFromFile("enemy_defeat.wav") ||
        !gameOverBuffer.loadFromFile("game_over.wav") ||
        !winBuffer.loadFromFile("win.wav")) {
        std::cout << "لم يتم العثور على ملفات الصوت!" << std::endl;
        // استمر في التنفيذ حتى لو لم يتم العثور على الأصوات
    }
    
    // إنشاء أدوات الصوت
    sf::Sound jumpSound, coinSound, enemyDefeatSound, gameOverSound, winSound;
    jumpSound.setBuffer(jumpBuffer);
    coinSound.setBuffer(coinBuffer);
    enemyDefeatSound.setBuffer(enemyDefeatBuffer);
    gameOverSound.setBuffer(gameOverBuffer);
    winSound.setBuffer(winBuffer);
    
    // موسيقى الخلفية - استخدام MP3 للموسيقى
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("background_music.mp3")) {
        std::cout << "لم يتم العثور على ملف موسيقى الخلفية!" << std::endl;
        // استمر في التنفيذ
    } else {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50); // ضبط مستوى الصوت
        backgroundMusic.play();
    }

    sf::Texture marioTexture;
    if (!marioTexture.loadFromFile("mario.png")) return -1;
    sf::Sprite mario(marioTexture);
    mario.setScale(0.0273f, 0.0507f);  // حجم ماريو بعد تكبيره بنسبة 30%
    mario.setPosition(50.f, 500.f);

    sf::Texture enemyTexture;
    if (!enemyTexture.loadFromFile("enemy.png")) return -1;

    float enemyHeight = enemyTexture.getSize().y * 0.0507f;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return -1;

    sf::Text gameTitle;
    gameTitle.setFont(font);
    gameTitle.setCharacterSize(24);
    gameTitle.setFillColor(sf::Color::White);
    gameTitle.setString("Super Mario Clone");
    gameTitle.setPosition(20, 50);

    sf::Text message, restartText;
    message.setFont(font);
    message.setCharacterSize(40);
    message.setFillColor(sf::Color::White);
    message.setPosition(300, 250);
    restartText.setFont(font);
    restartText.setCharacterSize(30);
    restartText.setString("Press R to Restart");
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(330, 320);

    bool gameOver = false, gameWin = false;
    bool soundPlayed = false; // متغير للتأكد من أن الصوت لم يتم تشغيله سابقاً
    sf::Vector2f velocity(0.f, 0.f);
    const float gravity = 0.5f;
    const float jumpStrength = -12.f;
    bool isOnGround = true;

    sf::RectangleShape ground(sf::Vector2f(2000.f, 100.f));
    ground.setPosition(0, 550);
    ground.setFillColor(sf::Color::Green);

    std::vector<sf::RectangleShape> obstacles;
    std::srand(static_cast<unsigned>(time(0)));
    float xPos = 200.f;
    while (xPos < 1800.f) {
        bool isBlack = (std::rand() % 6 == 0);
        sf::RectangleShape obs;
        if (isBlack) {
            obs.setSize(sf::Vector2f(60.f, 7.5f));
            obs.setFillColor(sf::Color::Black);
        } else {
            obs.setSize(sf::Vector2f(40.f, 15.f));
            obs.setFillColor(sf::Color::Red);
        }
        float yLevel = 350.f + rand() % 200;
        obs.setPosition(xPos, yLevel);
        obstacles.push_back(obs);
        xPos += static_cast<float>(30 + rand() % 50);
    }

    std::vector<Enemy> enemies;
    for (int i = 0; i < 5; ++i) {
        float enemyX = 300.f + i * 350.f;
        float enemyY = 550.f - enemyHeight;
        float rangeMin = enemyX - 100.f;
        float rangeMax = enemyX + 100.f;
        enemies.emplace_back(enemyTexture, enemyX, enemyY, 1.0f, rangeMin, rangeMax);
    }

    std::vector<sf::CircleShape> coins;
    int totalCoins = 10, collectedCoins = 0;
    for (int i = 0; i < totalCoins; ++i) {
        sf::CircleShape coin(10.f);
        coin.setFillColor(sf::Color::Yellow);
        float x = 250.f + rand() % 1600;
        float y = 400.f + rand() % 100;
        coin.setPosition(x, y);
        coins.push_back(coin);
    }

    sf::RectangleShape barBack(sf::Vector2f(200.f, 20.f));
    barBack.setFillColor(sf::Color(100, 100, 100));
    barBack.setPosition(20.f, 20.f);
    sf::RectangleShape barFill(sf::Vector2f(0.f, 20.f));
    barFill.setFillColor(sf::Color::Green);
    barFill.setPosition(20.f, 20.f);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(240.f, 20.f);
    scoreText.setString("Score: 0");
    int score = 0;

    sf::Texture finishTexture;
    sf::Sprite finishSprite;
    if (!finishTexture.loadFromFile("finish.png")) return -1;
    finishSprite.setTexture(finishTexture);
    finishSprite.setScale(0.15f, 0.3f);
    finishSprite.setPosition(1900.f, 400.f);

    sf::CircleShape sun(50.f);
    sun.setFillColor(sf::Color::Yellow);
    sun.setPosition(900.f, 50.f);

    sf::View view = window.getDefaultView();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if ((gameOver || gameWin) && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                mario.setPosition(50.f, 500.f);
                velocity = {0.f, 0.f};
                gameOver = gameWin = false;
                collectedCoins = score = 0;
                scoreText.setString("Score: 0");
                barFill.setSize(sf::Vector2f(0.f, 20.f));
                view.setCenter(500.f, 300.f);
                
                // إعادة تعيين الأعداء
                for (auto& enemy : enemies) {
                    enemy.sprite.setPosition(enemy.minX + 100, 550.f - enemyHeight);
                    enemy.sprite.setRotation(0);
                    enemy.defeated = false;
                }
                
                // إعادة تشغيل موسيقى الخلفية وإعادة تعيين متغير الصوت
                backgroundMusic.play();
                soundPlayed = false;
            }
        }

        if (!gameOver && !gameWin) {
            // التحرك باستخدام المفاتيح
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                mario.move(-5.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                mario.move(5.f, 0.f);
            
            // القفز عند الضغط على المسطرة
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isOnGround) {
                velocity.y = jumpStrength;
                isOnGround = false;
                jumpSound.play(); // تشغيل صوت القفز
            }
            
            // تطبيق الجاذبية
            velocity.y += gravity;
            mario.move(0.f, velocity.y);
            
            // فحص الاصطدام بالأرض
            if (mario.getPosition().y + mario.getGlobalBounds().height > ground.getPosition().y) {
                mario.setPosition(mario.getPosition().x, ground.getPosition().y - mario.getGlobalBounds().height);
                velocity.y = 0.f;
                isOnGround = true;
            }
            
            // تحديث الأعداء
            for (auto& enemy : enemies) {
                enemy.update();
                
                // التصادم مع العدو
                if (!enemy.defeated && mario.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                    if (velocity.y > 0 && mario.getPosition().y + mario.getGlobalBounds().height < 
                        enemy.sprite.getPosition().y + enemy.sprite.getGlobalBounds().height / 2) {
                        // هزمت العدو من فوق
                        enemy.defeat();
                        velocity.y = -8.f; // ارتداد صغير
                        score += 100;
                        scoreText.setString("Score: " + std::to_string(score));
                        enemyDefeatSound.play(); // تشغيل صوت هزيمة العدو
                    } else if (!enemy.defeated) {
                        // خسرت من العدو
                        gameOver = true;
                        message.setString("Game Over!");
                        if (!soundPlayed) {
                            gameOverSound.play(); // تشغيل صوت خسارة اللعبة
                            backgroundMusic.stop(); // إيقاف موسيقى الخلفية
                            soundPlayed = true;
                        }
                    }
                }
            }
            
            // جمع العملات
            for (size_t i = 0; i < coins.size(); ++i) {
                if (mario.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                    coins.erase(coins.begin() + i);
                    collectedCoins++;
                    score += 50;
                    scoreText.setString("Score: " + std::to_string(score));
                    coinSound.play(); // تشغيل صوت جمع العملة
                    
                    // تحديث شريط التقدم
                    float progressWidth = 200.f * static_cast<float>(collectedCoins) / totalCoins;
                    barFill.setSize(sf::Vector2f(progressWidth, 20.f));
                    
                    break;
                }
            }
            
            // الاصطدام بالعوائق
            for (auto& obs : obstacles) {
                if (mario.getGlobalBounds().intersects(obs.getGlobalBounds())) {
                    if (obs.getFillColor() == sf::Color::Black) {
                        // العائق الأسود يتسبب في خسارة اللعبة
                        gameOver = true;
                        message.setString("Game Over!");
                        if (!soundPlayed) {
                            gameOverSound.play(); // تشغيل صوت خسارة اللعبة
                            backgroundMusic.stop(); // إيقاف موسيقى الخلفية
                            soundPlayed = true;
                        }
                    } else {
                        // تصحيح موضع ماريو حسب اتجاه الاصطدام
                        sf::FloatRect marioBounds = mario.getGlobalBounds();
                        sf::FloatRect obsBounds = obs.getGlobalBounds();
                        
                        float overlapLeft = marioBounds.left + marioBounds.width - obsBounds.left;
                        float overlapRight = obsBounds.left + obsBounds.width - marioBounds.left;
                        float overlapTop = marioBounds.top + marioBounds.height - obsBounds.top;
                        float overlapBottom = obsBounds.top + obsBounds.height - marioBounds.top;
                        
                        // اختيار الاتجاه مع أقل تداخل
                        if (overlapTop < std::min({overlapBottom, overlapLeft, overlapRight})) {
                            mario.setPosition(mario.getPosition().x, obsBounds.top - marioBounds.height);
                            velocity.y = 0.f;
                            isOnGround = true;
                        } else if (overlapBottom < std::min({overlapTop, overlapLeft, overlapRight})) {
                            mario.setPosition(mario.getPosition().x, obsBounds.top + obsBounds.height);
                            velocity.y = 0.1f;
                        } else if (overlapLeft < overlapRight) {
                            mario.setPosition(obsBounds.left - marioBounds.width, mario.getPosition().y);
                        } else {
                            mario.setPosition(obsBounds.left + obsBounds.width, mario.getPosition().y);
                        }
                    }
                }
            }
            
            // الوصول إلى خط النهاية
            if (mario.getGlobalBounds().intersects(finishSprite.getGlobalBounds())) {
                gameWin = true;
                message.setString("You Win!");
                if (!soundPlayed) {
                    winSound.play(); // تشغيل صوت الفوز
                    backgroundMusic.stop(); // إيقاف موسيقى الخلفية
                    soundPlayed = true;
                }
            }
            
            // تحديث العرض للمتابعة مع اللاعب
            if (mario.getPosition().x > view.getCenter().x)
                view.setCenter(mario.getPosition().x, view.getCenter().y);
        }
        
        window.clear(sf::Color(135, 206, 235)); // لون السماء
        window.setView(view);
        
        // رسم العناصر
        window.draw(sun);
        window.draw(ground);
        
        for (auto& obs : obstacles)
            window.draw(obs);
            
        for (auto& coin : coins)
            window.draw(coin);
            
        for (auto& enemy : enemies)
            if (!enemy.defeated || enemy.defeatClock.getElapsedTime().asSeconds() <= 0.5f)
                window.draw(enemy.sprite);
                
        window.draw(finishSprite);
        window.draw(mario);
        
        // إعادة العرض إلى الوضع الافتراضي للواجهة
        sf::View fixedView = window.getDefaultView();
        window.setView(fixedView);
        
        window.draw(barBack);
        window.draw(barFill);
        window.draw(scoreText);
        window.draw(gameTitle);
        
        if (gameOver || gameWin) {
            window.draw(message);
            window.draw(restartText);
        }
        
        window.display();
    }

    return 0;
}
