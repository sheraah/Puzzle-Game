#pragma once
#include<iostream>
#include "sfLine.h"



//class for every Piece
class Piece
{
private:

	sf::RectangleShape Shape;
	sf::IntRect StartRect;
	sf::IntRect CurrRect;
	 
public:
    Piece(const unsigned int& x, const unsigned int& y, const unsigned int& ColX, const unsigned int& ColY, const sf::Texture& tex, 
		const unsigned int& WindowWidth, const unsigned int& WindowHeight)
	{
		//Calculating the Pos, Rect and more
		float Width = tex.getSize().x / ColX;
		float Height = tex.getSize().y / ColY;
		float ToWindowWidth = WindowWidth / ColX;
		float ToWindowHeight = WindowHeight / ColY;
		Shape.setTexture(&tex);
		Shape.setSize({ToWindowWidth, ToWindowHeight});
		StartRect = sf::IntRect(x * Width, y * Height, Width, Height);
		CurrRect = StartRect;
		Shape.setTextureRect(CurrRect);
		Shape.setPosition(x * ToWindowWidth, y * ToWindowHeight);
	}

	void draw(sf::RenderTarget& window, const bool& DrawLines)
	{
		//draw Shape and the lines
		window.draw(Shape);

		if (DrawLines)
		{
			sf::Vector2f BottomLeft = { Shape.getPosition().x, Shape.getPosition().y + Shape.getSize().y };
			sf::Vector2f BottomRight = { Shape.getPosition().x + Shape.getSize().x, Shape.getPosition().y + Shape.getSize().y };
			sf::Vector2f TopRight = { Shape.getPosition().x + Shape.getSize().x, Shape.getPosition().y };

			window.draw(sfLine(BottomLeft, BottomRight, sf::Color::Black, 6.f));
			window.draw(sfLine(TopRight, BottomRight, sf::Color::Black, 6.f));
		}
	}
	
	void SwapPiece(Piece& P)
	{
		//Swap the Texure Rects, but keep the StartRect, u can check if the StartRect = CurrRect that means the Rect wasnt changed
		sf::IntRect Temp = this->CurrRect;
		this->CurrRect = P.CurrRect;
		P.CurrRect = Temp;

		this->Shape.setTextureRect(this->CurrRect);
		P.Shape.setTextureRect(P.CurrRect);
	}

	//the function for checking if the Piece was swaped
	inline const bool Changed() const { return this->StartRect != this->CurrRect; };
};


//Puzzle class
class Puzzle
{
private:
	//Some Variables
	unsigned int ColX;
	unsigned int ColY;
	sf::Texture tex;

	std::vector<std::vector<Piece*>> Pieces;
	sf::RenderWindow* window;
	sf::Event evnt;
	bool Solved = false;

	void randomize()
	{
		 for (int y = 0; y < ColY; y++)
		 	for (int x = 0; x < ColX; x++)
		 	{
				//Generate a x between ColX, y between ColY and if the generated num is equal to the current x or y generate it again. 
				//if u have  a x and y now swap the current x and y with the generated x and y.
		 		int rx = rand() % ColX;
		 		int ry = rand() % ColY;
		 
		 		while (rx == x && ry == y)
		 		{
		 			rx = rand() % ColX;
		 			ry = rand() % ColY;
		 		}
				Pieces[y][x]->SwapPiece(*Pieces[ry][rx]);
		 	}	
	}


	void update()
	{
		// update events
		while (window->pollEvent(evnt))
		{
			switch (evnt.type)
			{
			case sf::Event::Closed:
				window->close();
				break;
			}
		}


		if (!Solved)
		{ 
		sf::Vector2f MousePos = sf::Vector2f(float(sf::Mouse::getPosition(*window).x), float(sf::Mouse::getPosition(*window).y));

		static int StartX = 0;
		static int StartY = 0;
		static bool canMouseBePressed = true;

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && canMouseBePressed)
		{
			//If the Mouse is pressed store the Mouse pos col pos in the static variables but do it once until the Mouse isnt released 
			StartX = MousePos.x / (window->getSize().x / ColX);
			StartY = MousePos.y / (window->getSize().y / ColY);
			canMouseBePressed = false;

		}
		if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !canMouseBePressed)
		{
			//if the Mouse is released get the Mouse Col Pos and Swap it with the start pos
			int EndX = MousePos.x / (window->getSize().x / ColX);
			int EndY = MousePos.y / (window->getSize().y / ColY);

			if (StartX >= 0 && StartX < ColX && StartY >= 0 && StartY < ColY &&
				EndX >= 0 && EndX < ColX && EndY >= 0 && EndY < ColY)
			{
				Pieces[StartY][StartX]->SwapPiece(*Pieces[EndY][EndX]);
			}

			canMouseBePressed = true;
		}


		//In total there are ColX * ColY Collumns so check with a Count Variable how many Pieces have thier StartRect
			size_t Count = 0;

			for (const auto& P : Pieces)
				for (const auto& p : P)
					if (!p->Changed()) Count++;

			if (Count == ColX * ColY) Solved = true;
		}
	}

	void render()
	{
		//draw all things

		window->clear();

		for (const auto& i : Pieces)
			for (const auto& j : i)
				j->draw(*window, !Solved);

		window->display();
	}


public:

	Puzzle(const unsigned int& ColX, const unsigned int& ColY, const std::string& PuzzlePath, const unsigned int& WindowWidth,
		const unsigned int& WindowHeight)
		:ColX(ColX), ColY(ColY)
	{
		//init all things
		window = new sf::RenderWindow(sf::VideoMode(WindowWidth, WindowHeight), "Puzzle", sf::Style::Close);
		window->setFramerateLimit(60);

		tex.loadFromFile(PuzzlePath);

		for (int y = 0; y < ColY; y++)
		{
			std::vector<Piece*> Row;
			for (int x = 0; x < ColX; x++)
			{
				Row.emplace_back(new Piece(x, y, ColX, ColY, tex, WindowWidth, WindowHeight));
			}
			Pieces.emplace_back(Row);
		}

		randomize();

	}
	~Puzzle()
	{
	   //delete all pointers

		for (const auto& P : Pieces)
			for (const auto& p : P)
				delete p;
	
		Pieces.clear();

		delete window;

		
	};


	void run()
	{
		//main loop
		while (window->isOpen())
		{
			update();

			render();
		}
	}
};

