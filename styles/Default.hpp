#pragma once

#include "Style.hpp"

#include <SFML/Graphics/CircleShape.hpp>

namespace TME::Styles {

class Default : public Style {
public:
	Default(sf::Vector2f const& scale, float stop_size, float line_width);
	virtual void draw_segment(sf::RenderTexture &rt, sf::Color const& line, sf::Vector2f const& position, int origin_id, int destination_id);
	virtual void draw_half_segment(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& square, int destination_id);
	virtual void draw_terminus_dot(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position);
	virtual void draw_station(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position);
	virtual void draw_station_transfer(sf::RenderTexture& rt, sf::Vector2f const& position);
	virtual void draw_station_large(sf::RenderTexture& rt, sf::Vector2f const& origin, sf::Vector2f const& destination);
	virtual void draw_station_terminus(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position);
	virtual void draw_center_circle(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position);

protected:

	int _center_circle_radius;
	// Horizontal rectangle
	sf::RectangleShape _rect_h;
	// Vertical rectangle
	sf::RectangleShape _rect_v;
	// Even diagonal (along the same sign for x and y) rectangle
	sf::RectangleShape _rect_ed;
	// Odd diagonal (along a different sign for x and y) rectangle
	sf::RectangleShape _rect_od;
	
	sf::CircleShape _terminus_dot;
	sf::CircleShape _transfer_station_bg;
	sf::CircleShape _transfer_station_fg;
	sf::CircleShape _simple_station;
	sf::CircleShape _center_circle;
	
	

};

}
