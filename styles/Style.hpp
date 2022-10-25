#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <cmath>

namespace TME::Styles {

class Style {
public:
	Style(sf::Vector2f const& scale, float stop_size, float line_width) :
	_scale(scale), _stop_radius(floor(((scale.x + scale.y) / 2) * stop_size / 2)), _line_width(line_width) {}
	virtual void draw_segment(sf::RenderTexture &rt, sf::Color const& line, sf::Vector2f const& position, int origin_id, int destination_id) = 0;
	virtual void draw_half_segment(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& origin, int destination_id) = 0;
	virtual void draw_terminus_dot(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) = 0;
	virtual void draw_station(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) = 0;
	virtual void draw_station_transfer(sf::RenderTexture& rt, sf::Vector2f const& position) = 0;
	virtual void draw_station_large(sf::RenderTexture& rt, sf::Vector2f const& origin, sf::Vector2f const& destination) = 0;
	virtual void draw_station_terminus(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) = 0;
	virtual void draw_center_circle(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) = 0;
	
protected:
	const sf::Vector2f _scale;
	const int _stop_radius;
	const float _line_width;
};

}
