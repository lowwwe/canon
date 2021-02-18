/// <summary>
/// @author Peter Lowe
/// @date Feb 2021
///
/// Simple canon game
/// </summary>

#include "Game.h"
#include <iostream>



/// <summary>
/// default constructor
/// setup the window properties
/// load and setup the text 
/// load and setup thne image
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ 800U, 600U, 32U }, "SFML Game" },
	m_exitGame{false} //when true game will exit
{
	setupFontAndText(); // load font 
	setupSprite(); // load texture
}

/// <summary>
/// default destructor we didn't dynamically allocate anything
/// so we don't need to free it, but mthod needs to be here
/// </summary>
Game::~Game()
{
}


/// <summary>
/// main game loop
/// update 60 times per second,
/// process update as often as possible and at least 60 times per second
/// draw as often as possible but only updates are on time
/// if updates run slow then don't render frames
/// </summary>
void Game::run()
{	
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame); //60 fps
			render();
		}
		render(); // as many as possible
	}
}
/// <summary>
/// handle user and system events/ input
/// get key presses/ mouse moves etc. from OS
/// and user :: Don't do game update here
/// </summary>
void Game::processEvents()
{
	sf::Event newEvent;	
	while (m_window.pollEvent(newEvent))
	{
		switch (newEvent.type)
		{
		case  sf::Event::Closed:
			m_exitGame = true;
			break;
		case sf::Event::KeyPressed:
			processKeys(newEvent);
			break;
		case  sf::Event::MouseButtonPressed:
			processMousePress(newEvent);
			break;
		case sf::Event::MouseMoved:
			processMouseMove(newEvent);
			break;
		case sf::Event::MouseButtonReleased:
			processMouseRelase(newEvent);
			break;
		default:
			break;
		}
		
	}
}


/// <summary>
/// deal with key presses from the user
/// </summary>
/// <param name="t_event">key press event</param>
void Game::processKeys(sf::Event t_event)
{
	switch (t_event.key.code)
	{
	case sf::Keyboard::Escape:
		m_exitGame = true;
		break;
	case sf::Keyboard::Subtract:
		adjustGravity(-0.005f);
		break;
	case sf::Keyboard::Add:
		adjustGravity(0.005f);
		break;
	case sf::Keyboard::F1:
		m_graphics = !m_graphics;
		break;

	}

	
}


/// <summary>
/// start the aiming process 
/// setup the line from ball to mouse
/// rotate the canon to match aimline
/// </summary>
/// <param name="t_event">mouse press event</param>
void Game::processMousePress(sf::Event t_event)
{
	if (!m_aiming && !m_firing)
	{
		m_mouseEnd.x = static_cast<float>(t_event.mouseButton.x);
		m_mouseEnd.y = static_cast<float>(t_event.mouseButton.y);
		m_aiming = true;
		m_aimLine.clear();
		setupAimLine();
	}
}

/// <summary>
/// update mouse location
/// setup the line from ball to mouse
/// rotate the canon to match aimline
/// </summary>
/// <param name="t_event">mouse move event</param>
void Game::processMouseMove(sf::Event t_event)
{
	if (m_aiming & !m_firing)
	{
		m_mouseEnd.x = static_cast<float>(t_event.mouseMove.x);
		m_mouseEnd.y = static_cast<float>(t_event.mouseMove.y);
		setupAimLine();
	}
}


/// <summary>
/// workout velocity
// clear aiming line
// set firing to true
/// </summary>
/// <param name="t_event">mouse release event</param>
void Game::processMouseRelase(sf::Event t_event)
{
	if (m_aiming && !m_firing)
	{
		m_aiming = false;
		m_firing = true;
		m_mouseEnd.x = static_cast<float>(t_event.mouseButton.x);
		m_mouseEnd.y = static_cast<float>(t_event.mouseButton.y);
		m_aimLine.clear();
		m_ballVelocity = m_mouseEnd - m_canonEnd;
		m_ballVelocity = m_ballVelocity / 50.0f;
	}
}


/// <summary>
/// add two vertex to line one for canon one for mouse
// work out angle of line and set rotation to tan inverse
/// </summary>
void Game::setupAimLine()
{
	sf::Vertex point;
	float angleradians;
	float angleDegrees;
	sf::Vector2f line;
	point.color = sf::Color::Black;

	m_aimLine.clear();
	point.position = m_mouseEnd;
	m_aimLine.append(point);
	point.position = m_canonEnd;
	m_aimLine.append(point);

	line = m_mouseEnd - m_canonEnd;
	angleradians = std::atan2(line.y, line.x);
	angleDegrees = angleradians * 180.0f / 3.14f;
	m_cannon.setRotation(angleDegrees + 90.0f);
	m_barrelSprite.setRotation(angleDegrees + 90.0f);
	m_bulletSprite.setRotation(angleDegrees + 90.0f);
}



/// <summary>
/// Update the game world
/// </summary>
/// <param name="t_deltaTime">time interval per frame</param>
void Game::update(sf::Time t_deltaTime)
{
	if (m_exitGame)
	{
		m_window.close();
	}
	moveTarget();
	animateGumba();
	if (m_firing)
	{
		moveBall();
		m_firing = checkGound(m_ballLocation);
		// once firing is set to false we don't want a different check setiing it back to true
		if (m_firing)
		{
			m_firing = checkCollision(m_ball, m_wall, false);
		}
		if (m_firing)
		{
			m_firing = checkCollision(m_ball, m_target, true);
		}
	}
}

/// <summary>
/// draw the frame and then switch buffers
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::White);
	if (m_graphics)
	{
		m_window.draw(m_backgroundSprite);
		m_window.draw(m_bulletSprite); //moved behind the barrell
		m_window.draw(m_barrelSprite);
		m_window.draw(m_baseSprite);
		m_window.draw(m_welcomeMessage);		
		m_window.draw(m_wallSprite);
		m_window.draw(m_arrowSprite);		
		m_window.draw(m_gumbaSprite);
		if (m_aiming)
		{
			drawAimLine();
		}
		m_window.draw(m_hitsMessage);
		m_window.draw(m_missesMessage);
	}
	else
	{
		m_window.draw(m_welcomeMessage);	
		m_window.draw(m_cannon);	
		m_window.draw(m_wall);
		m_window.draw(m_gravityBar);		
		m_window.draw(m_target);		
		if (m_aiming)
		{
			m_window.draw(m_aimLine);
		}
		m_window.draw(m_ball);
		m_window.draw(m_hitsMessage);
		m_window.draw(m_missesMessage);
	}
 	m_window.display();
}


/// <summary>
/// update the balls sposition and update the velocity
/// adjust the angle of the bullet to match it's new
///	velocity
/// </summary>
void Game::moveBall()
{
	float angle;
	m_ballVelocity += m_gravity;
	m_ballLocation += m_ballVelocity;
	m_ball.setPosition(m_ballLocation);
	m_bulletSprite.setPosition(m_ballLocation);
	angle = std::atan2(m_ballVelocity.y, m_ballVelocity.x);
	angle = angle * 180.0f / 3.14f;
	m_bulletSprite.setRotation(angle + 90.0f);
}


/// <summary>
/// add velocity to target, flip direction when boundry met
// scale gumba by -1 and move origin to draw in orignal location
/// </summary>
void Game::moveTarget()
{
	const float LEFT_EDGE = 420.0f;
	const float RIGHT_EDGE = 740.0f;

	if (m_targetLocation.x < LEFT_EDGE)
	{
		m_targetVelocity.x = 0.6f;
		m_gumbaSprite.setScale(-1.0f, 1.0f);
		m_gumbaSprite.setOrigin(52.0f, 0.0f);
	}
	if (m_targetLocation.x > RIGHT_EDGE)
	{
		m_targetVelocity.x = -0.6f;
		m_gumbaSprite.setScale(1.0f, 1.0f);
		m_gumbaSprite.setOrigin(0.0f, 0.0f);
	}
	m_targetLocation += m_targetVelocity;
	m_target.setPosition(m_targetLocation);
	m_gumbaSprite.setPosition(m_targetLocation);
}


/// <summary>
/// check if bullet has left by three sides
/// </summary>
/// <param name="t_location">location of centre of ball</param>
/// <returns>false if below ground</returns>
bool Game::checkGound(sf::Vector2f t_location)
{
	if (t_location.y > 600.0f || (t_location.x < 0.0f || t_location.x > 800.f))
	{
		m_ballLocation = sf::Vector2f{ 100.0f,550.0f };
		m_ball.setPosition(m_ballLocation);
		m_bulletSprite.setPosition(m_ballLocation);
		m_misses++;
		m_missesMessage.setString("Misses " + std::to_string(m_misses));
		return false;
	}
	return true;
}

/// <summary>
/// increment frame counter and cast to int
// when frame changes adjust texture rect
// when last frame reached rest to zero
/// </summary>
void Game::animateGumba()
{
	const int CELL_WIDTH = 52;
	const int CELL_HEIGHT = 54;
	int frame;
	m_gumbaFrameCounter += m_gumbaFrameIncrement;
	frame = static_cast<int>(m_gumbaFrameCounter);
	if (frame >= m_gumbaLastFrame)
	{
		frame = 0;
		m_gumbaFrameCounter = 0.0f;
	}
	if (frame != m_gumbaFrame)
	{
		m_gumbaFrame = frame;
		m_gumbaSprite.setTextureRect(sf::IntRect(frame * CELL_WIDTH, 0, CELL_WIDTH, CELL_HEIGHT));
	}
}

/// <summary>
/// draw a sequence of bullets following the path of the bullet
/// each bullet should get progressivly mnore transparnt
/// </summary>
void Game::drawAimLine()
{
	sf::Vector2f velocity; // local
	sf::Vector2f location = m_ballLocation; // local copy never change actual bullet location
	float angle;
	sf::Color colour{255u, 255u, 255u, 255u}; // initally white full opaque / no transparent
	velocity = (m_mouseEnd - m_canonEnd)/50.0f;
	for (int i = 0; i < 255; i++)
	{
		velocity += m_gravity;
		location += velocity;
		if (i % 20 == 0) // draw bullet every 1/3 of a second [60fps]
		{
			angle = std::atan2(velocity.y, velocity.x);
			angle = angle * 180.0f / 3.14f;
			m_aimBulletSprite.setRotation(angle + 90.0f);
			m_aimBulletSprite.setPosition(location);
			colour.a = 255 - i; // decrease opaqueness == increase transparency
			m_aimBulletSprite.setColor(colour);
			m_window.draw(m_aimBulletSprite); // sprite drawn multiple time in single frame [gameplay]
		}

	}
	
}

/// <summary>
/// load the font and setup the text message for screen
/// </summary>
void Game::setupFontAndText()
{
	if (!m_ArialBlackfont.loadFromFile("ASSETS\\FONTS\\ariblk.ttf"))
	{
		std::cout << "left mouse click to load, release to fire " << std::endl;
	}
	m_welcomeMessage.setFont(m_ArialBlackfont);
	m_welcomeMessage.setString("left mouse click to load, release to fire");
	m_welcomeMessage.setStyle( sf::Text::Italic );
	m_welcomeMessage.setPosition(40.0f, 40.0f);
	m_welcomeMessage.setCharacterSize(30U);
	m_welcomeMessage.setOutlineColor(sf::Color::Black);
	m_welcomeMessage.setFillColor(sf::Color::Yellow);
	m_welcomeMessage.setOutlineThickness(1.0f);

	m_hitsMessage.setFont(m_ArialBlackfont);
	m_hitsMessage.setString("Hits");
	m_hitsMessage.setPosition(40.0f, 80.0f);
	m_hitsMessage.setCharacterSize(20U);
	m_hitsMessage.setFillColor(sf::Color::Blue);
	
	m_missesMessage.setFont(m_ArialBlackfont);
	m_missesMessage.setString("Misses");
	m_missesMessage.setPosition(40.0f, 110.0f);
	m_missesMessage.setCharacterSize(20U);
	m_missesMessage.setFillColor(sf::Color::Blue);

}

/// <summary>
/// load the textures and setup the sprites
/// </summary>
void Game::setupSprite()
{
	if (!m_gumbaTexture.loadFromFile("assets\\images\\gumba.png"))
	{
		std::cout << "error gumba file" << std::endl;
	}
	m_gumbaSprite.setTexture(m_gumbaTexture);
	m_gumbaSprite.setScale(-1.0f, 1.0f);
	m_gumbaSprite.setOrigin(52.0f, 0.0f);
	m_gumbaSprite.setPosition(m_targetLocation);
	animateGumba(); // set first frame

	m_cannon.setFillColor(sf::Color::Black);
	m_cannon.setSize(sf::Vector2f{ 20.0f,70.0f });
	m_cannon.setPosition(sf::Vector2f{ 100.0f,550.0f });
	m_cannon.setOrigin(10.0f, 35.0f);
	m_cannon.setRotation(45.0f);
	if (!m_canonBaseTexture.loadFromFile("ASSETS\\IMAGES\\base.PNG"))
	{
		std::cout << "error base file" << std::endl;
	}
	m_baseSprite.setTexture(m_canonBaseTexture);
	m_baseSprite.setPosition(70.0f, 531.0f);
	if (!m_canonBarrellTexture.loadFromFile("ASSETS\\IMAGES\\BARREL.PNG"))
	{
		std::cout << "error BARREL file" << std::endl;
	}
	m_barrelSprite.setTexture(m_canonBarrellTexture);
	m_barrelSprite.setOrigin(22.0f, 45.0f);
	m_barrelSprite.setRotation(45.0f);
	m_barrelSprite.setPosition(100.0f, 550.0f);

	if (!m_bulletTexture.loadFromFile("assets\\images\\bullet.png"))
	{
		std::cout << "error bullet file" << std::endl;
	}
	m_bulletSprite.setTexture(m_bulletTexture);
	m_bulletSprite.setOrigin(20.0f, 20.0f);
	m_bulletSprite.setRotation(45.0f);
	m_aimBulletSprite.setTexture(m_bulletTexture);
	m_aimBulletSprite.setOrigin(20.0f, 20.0f);
	m_aimBulletSprite.setRotation(45.0f);
	m_bulletSprite.setPosition(100.0f, 550.0f);

	m_wall.setFillColor(sf::Color::Black);
	m_wall.setSize(sf::Vector2f{ 32.0f,100.0f });
	m_wall.setPosition(sf::Vector2f{ 400.0f,500.0f });
	if (!m_wallTexture.loadFromFile("ASSETS\\IMAGES\\WALL.JPG"))
	{
		std::cout << "error wall file" << std::endl;
	}
	m_wallTexture.setRepeated(true);  // repeated texture
	m_wallSprite.setTexture(m_wallTexture);
	m_wallSprite.setTextureRect(sf::IntRect(0, 0, 32, 100));
	m_wallSprite.setPosition(m_wall.getPosition());

	m_gravityBar.setFillColor(sf::Color::Blue);
	m_gravityBar.setSize(sf::Vector2f{ 20.0f,100.0f });
	m_gravityBar.setPosition(sf::Vector2f{ 760.0f,40.0f });
	if (!m_arrowTexture.loadFromFile("assets\\images\\arrow.png"))
	{
		std::cout << "problem with arrow" << std::endl;
	}
	m_arrowSprite.setTexture(m_arrowTexture);
	m_arrowSprite.setPosition(sf::Vector2f{ 760.0f,40.0f });
	
	m_target.setFillColor(sf::Color::Green);
	m_target.setSize(sf::Vector2f{ 55.0f,55.0f });
	m_targetLocation = sf::Vector2f{ 420.0f,545.0f };
	m_target.setPosition(m_targetLocation);
	

	m_ball.setFillColor(sf::Color::Red);
	m_ball.setRadius(10.0f);
	m_ballLocation = sf::Vector2f{ 100.0f,550.0f };
	m_ball.setPosition(m_ballLocation);	
	m_ball.setOrigin(10.0f, 10.0f);

	if (!m_backgroundTexture.loadFromFile("ASSETS\\IMAGES\\background.jpg"))
	{
		std::cout << "problem with background" << std::endl;
	}
	m_backgroundSprite.setTexture(m_backgroundTexture);
	m_backgroundSprite.setPosition(0.0f, 0.0f);
}

/// <summary>
/// allow = & - to changew gravity value
/// adjust bar or arrow to represent new value
/// </summary>
/// <param name="t_amount">amount to change</param>
void Game::adjustGravity(float t_amount)
{
	float magnitude;
	m_gravity.y += t_amount;
	m_gravityBar.setFillColor(sf::Color::Blue);
	m_arrowSprite.setColor(sf::Color::White);
	if (m_gravity.y < 0.0f) // never let gravity become negative
	{
		m_gravity.y = 0.0f;
		m_gravityBar.setFillColor(sf::Color::Red);
		m_arrowSprite.setColor(sf::Color::Blue);
	}
	magnitude = m_gravity.y * 500.0f + 50;
	m_gravityBar.setSize(sf::Vector2f(20.0f, magnitude));
	m_arrowSprite.setScale(1.0f, (m_gravity.y * 15.0f )+ 1.0f);

}


/// <summary>
/// check if the ball intersects with the target or wall
/// </summary>
/// <param name="t_circle">ball / bullet</param>
/// <param name="t_rectangle">wall / target</param>
/// <param name="m_target">true if target</param>
/// <returns>false if hit</returns>
bool Game::checkCollision(sf::CircleShape& t_circle, sf::RectangleShape& t_rectangle, bool t_target)
{
	bool result = true;
	sf::FloatRect circle = t_circle.getGlobalBounds();;
	sf::FloatRect rectangle = t_rectangle.getGlobalBounds();
	if (circle.intersects(rectangle))
	{
		m_ballLocation = sf::Vector2f{ 100.0f,550.0f };
		m_ball.setPosition(m_ballLocation);
		m_bulletSprite.setPosition(m_ballLocation);
		if (t_target)
		{
			m_hits++;
			m_hitsMessage.setString("Hits " + std::to_string(m_hits));
		}
		else
		{
			m_misses++;
			m_missesMessage.setString("Misses " + std::to_string(m_misses));
		}
		result = false;
	}
	return result;
	
}



