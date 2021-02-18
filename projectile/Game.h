/// <summary>
/// author Pete Lowe Feb 2021
/// you need to change the above line or lose marks
/// </summary>
#ifndef GAME_HPP
#define GAME_HPP
/// <summary>
/// include guards used so we don't process this file twice
/// same as #pragma once
/// Don't forget the endif at the bottom
/// </summary>
#include <SFML/Graphics.hpp>

class Game
{
public:
	Game();
	~Game();
	/// <summary>
	/// main method for game
	/// </summary>
	void run();

private:

	void processEvents();
	void processKeys(sf::Event t_event);
	void processMousePress(sf::Event t_event);
	void processMouseMove(sf::Event t_event);
	void processMouseRelase(sf::Event t_event);
	void setupAimLine();
	void update(sf::Time t_deltaTime);
	void render();
	void moveBall();
	void moveTarget();
	bool checkGound(sf::Vector2f t_location);
	
	void setupFontAndText();
	void setupSprite();
	void adjustGravity(float t_amount);
	bool checkCollision(sf::CircleShape& t_circle, sf::RectangleShape& t_rectangle, bool t_target);


	sf::RenderWindow m_window; // main SFML window
	sf::Font m_ArialBlackfont; // font used by message
	sf::Text m_welcomeMessage; // text used for message on screen
	sf::Text m_hitsMessage; // text for hits made
	sf::Text m_missesMessage; // text for shots missed
	int m_hits{ 0 };  // hits made
	int m_misses{ 0 }; // misses counter
	bool m_exitGame; // control exiting game
	sf::RectangleShape m_cannon; // cannon representation
	sf::RectangleShape m_wall; // obstical to overcome
	sf::RectangleShape m_gravityBar; // bar to display value of gravity
	sf::RectangleShape m_target;  // rectangle target
	sf::CircleShape  m_ball; // ball representation
	sf::VertexArray m_aimLine{ sf::Lines }; // aiminmg line
	bool m_aiming{ false }; // are we aiming
	bool m_firing{ false }; // is the ball in the air
	sf::Vector2f m_targetLocation; // top left of target
	sf::Vector2f m_targetVelocity{ 0.6f,0.0f }; // left or right speed of target
	
	sf::Vector2f m_ballLocation{ 0.0f,0.0f }; // ball position
	sf::Vector2f m_ballVelocity{ 0.0f,0.0f }; // ball velocity
	sf::Vector2f m_gravity{ 0.0f,0.05f }; // gravity
	sf::Vector2f m_canonEnd { 100.0f,550.0f }; // mouse down
	sf::Vector2f m_mouseEnd{ 0.0f,0.0f }; // mouse up


	/// <summary>
	/// Extra elements needed for graphics version
	/// </summary>

	void animateGumba();
	void drawAimLine();

	bool m_graphics{ false }; // to control if graphics are on
	
	sf::Texture m_bulletTexture; // texture for bullet
	sf::Sprite m_bulletSprite; // sprite for bullet
	sf::Texture m_backgroundTexture; // texture for background
	sf::Sprite m_backgroundSprite; // sprite for background
	sf::Texture m_canonBaseTexture; // texture for cannon base
	sf::Texture m_canonBarrellTexture; // texture for canon barrell
	sf::Sprite m_barrelSprite;  // sprite for canon barrel
	sf::Sprite m_baseSprite; // sprite for canon base
	sf::Texture m_wallTexture; // texture for wall obstical
	sf::Sprite m_wallSprite; // sprite for wall
	sf::Sprite m_aimBulletSprite; // sprite for aiming line

	sf::Texture m_arrowTexture; // texture for gravity arrow
	sf::Sprite m_arrowSprite; // sprite for gravity arrow
	sf::Texture m_gumbaTexture; // texture sheet for target gumba
	sf::Sprite m_gumbaSprite; // sprite for gumba
	int m_gumbaFrame{ -1 }; // current animation frame : -1 will trigger a new frame
	float m_gumbaFrameCounter{ 0.0f }; // accumulating counter for frame
	float m_gumbaFrameIncrement{ 0.3f }; // increment amount for frame per game step
	int m_gumbaLastFrame{ 20 }; // number of frames in animation

};

#endif // !GAME_HPP

