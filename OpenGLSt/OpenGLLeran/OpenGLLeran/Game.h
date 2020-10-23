/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#ifndef  GAME_H
#define GAME_H
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"
//#include "power_up.h"

// Represents the current state of the game
enum  GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};
// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f,20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
// Initial velocity of the player paddle
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f,-350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

class Game
{
public:
	// 游戏状态
	GameState State;
	GLboolean Keys[1024];
	GLuint Width, Height;
	std::vector<GameLevel> Levels;
	unsigned int            Level;
	unsigned int            Lives;
	// 构造函数/析构函数
	Game(GLuint width,GLuint height);
	~Game();
	// 初始化游戏状态（加载所有的着色器/纹理/关卡）
	void Init();
	// 游戏循环
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
	void DoCollisions();
	// reset
	void ResetLevel();

};

#endif // ! GAME_H

