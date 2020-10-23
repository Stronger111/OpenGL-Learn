/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "Game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "ball_object.h"
// Game-related State data
SpriteRenderer *Renderer;
GameObject     *Player;
BallObject     *Ball;

// collision detection
bool CheckCollision(GameObject &one, GameObject &two);
bool CheckCollision(BallObject &one, GameObject &two);
Game::Game(GLuint width,GLuint height):State(GAME_ACTIVE),Keys(),Width(width),Height(height)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
}

void Game::Init()
{
	// load shaders
	ResourceManager::LoadShader("Shader/Breakout/sprite.vs", "Shader/Breakout/sprite.fs", nullptr, "sprite");
	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// 设置专用于渲染的控制
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	// 加载纹理
	ResourceManager::LoadTexture(("resources/textures/background.jpg"), false, "background");
	ResourceManager::LoadTexture(("resources/textures/awesomeface.png"), true, "face");
	ResourceManager::LoadTexture(("resources/textures/block.png"), false, "block");
	ResourceManager::LoadTexture(("resources/textures/block_solid.png"), false, "block_solid");
	ResourceManager::LoadTexture(("resources/textures/paddle.png"), true, "paddle");
	// 加载关卡
	// load levels
	GameLevel one; one.Load(("resources/levels/one.lvl"), this->Width, this->Height / 2);
	GameLevel two; two.Load(("resources/levels/two.lvl"), this->Width, this->Height / 2);
	GameLevel three; three.Load(("resources/levels/three.lvl"), this->Width, this->Height / 2);
	GameLevel four; four.Load(("resources/levels/four.lvl"), this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;
	// configure game objects
	glm::vec2 playerPos= glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x/2.0f-BALL_RADIUS,-BALL_RADIUS*2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::Update(GLfloat dt)
{
	// 更新对象
	Ball->Move(dt, this->Width);
	// 检测碰撞
	this->DoCollisions();
}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State==GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY*dt;
		//移动挡板
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
				{
					Ball->Position.x += velocity;
				}
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}
}

void Game::Render()
{
	/*Renderer->DrawSprite(ResourceManager::GetTexture("face"),
		glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));*/
	if (this->State==GAME_ACTIVE)
	{
		// 绘制背景
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		// 绘制关卡
		this->Levels[this->Level].Draw(*Renderer);
		// draw player
		Player->Draw(*Renderer);
		// draw Ball
		Ball->Draw(*Renderer);
	}
}

void Game::DoCollisions()
{
	for (GameObject &box:this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			if (CheckCollision(*Ball,box))
			{
				if (!box.IsSolid)
					box.Destroyed = GL_TRUE;
			}
		}
	}
}



bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
	// collision x-axis?
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	// collision y-axis?
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// collision only if on both axes
	return collisionX&&collisionY;
}

bool CheckCollision(BallObject &one,GameObject &two)  // AABB - Circle collision
{
	// 获取圆的中心 
	glm::vec2 center(one.Position+one.Radius);
	// 计算AABB的信息（中心、半边长）
	glm::vec2 aabb_half_extents(two.Size.x/2,two.Size.y/2);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y); \
	// 获取两个中心的差矢量
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
	glm::vec2 closest = aabb_center + clamped;
	// 获得圆心center和最近点closest的矢量并判断是否 length <= radius
	difference = closest - center;
	return glm::length(difference) < one.Radius;
}
