#include "Default.hpp"
#include "RoundedRectangleShape.hpp"

namespace TME::Styles {

Default::Default(sf::Vector2f const& scale, float stop_size, float line_width) 
	: Style(scale, stop_size, line_width),
	_center_circle_radius(floor(((scale.x + scale.y) / 2) * line_width / 4) * 2),
	_rect_h(sf::Vector2f(scale.x / 2, _center_circle_radius * 2) ),
	_rect_v(sf::Vector2f(_center_circle_radius * 2, scale.y / 2)),
	_rect_ed(sf::Vector2f(sqrt(pow(scale.x / 2, 2) + pow(scale.y / 2, 2)), _center_circle_radius * 2)),
	//_rect_h(sf::Vector2f(scale.x / 2, floor(line_width * scale.y / 2) * 2.0f) ),
	//_rect_v(sf::Vector2f(floor(line_width * scale.x / 2) * 2.0f, scale.y / 2)),
	//_rect_ed(sf::Vector2f(sqrt(pow(scale.x / 2, 2) + pow(scale.y / 2, 2)),
	//line_width * sqrt(pow(scale.x / 2, 2) + pow(scale.y / 2, 2)))),
	_rect_od(_rect_ed) {
		
		_rect_h.setOrigin(0, floor(_rect_h.getLocalBounds().height / 2));
		_rect_v.setOrigin(floor(_rect_h.getLocalBounds().width / 2), 0);
		_rect_ed.setOrigin(0, floor(_rect_h.getLocalBounds().height / 2));
		_rect_od.setOrigin(0, floor(_rect_h.getLocalBounds().height / 2));
		
		_rect_ed.setRotation(45);
		_rect_od.setRotation(-45);	
			
		_terminus_dot.setRadius(_stop_radius / 2);
		_terminus_dot.setOrigin(_stop_radius / 2, _stop_radius / 2);
		
		_transfer_station_bg.setRadius(_stop_radius);
		_transfer_station_bg.setOrigin(_stop_radius, _stop_radius);
		_transfer_station_bg.setFillColor(sf::Color::Black);
		
		_transfer_station_fg.setRadius(_stop_radius * 0.9);
		_transfer_station_fg.setOrigin(_stop_radius * 0.9, _stop_radius * 0.9);
		_transfer_station_fg.setFillColor(sf::Color::White);
		
		_simple_station.setRadius(_stop_radius);
		_simple_station.setOrigin(_stop_radius, _stop_radius);
		
		
		_center_circle.setRadius(_center_circle_radius);
		_center_circle.setOrigin(sf::Vector2f(_center_circle_radius, _center_circle_radius));
}

void Default::draw_segment(sf::RenderTexture &rt, sf::Color const& line, sf::Vector2f const& position, int origin_id, int destination_id) {
	draw_half_segment(rt, line, position, origin_id);
	draw_half_segment(rt, line, position, destination_id);
}

void Default::draw_half_segment(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& square, int destination_id) {
	sf::Vector2f center = sf::Vector2f((square.x + 0.5) * _scale.x, (square.y + 0.5) * _scale.y);
	sf::RectangleShape rect;
	switch(destination_id) {
	case 0:
	case 8:
		rect = _rect_ed;
		//rect.setFillColor(sf::Color::Black);
		break;
	case 1:
	case 7:
		rect = _rect_h;
		//rect.setFillColor(sf::Color::Blue);
		break;
	case 2:
	case 6:
		rect = _rect_od;
		//rect.setFillColor(sf::Color::Yellow);
		break;
	case 3:
	case 5:
		rect = _rect_v;
		//rect.setFillColor(sf::Color::Red);
		break;
	default: // Should not happen
		return;
	}
	
	if(destination_id <= 3)
			rect.setRotation(rect.getRotation() + 180);
	
	rect.setPosition(center);
	rect.setFillColor(line);
	rt.draw(rect);
}

void Default::draw_terminus_dot(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) {
	sf::CircleShape td = _terminus_dot;
	sf::Vector2f center = position + sf::Vector2f(0.5, 0.5);
	td.setPosition(center.x * _scale.x, center.y * _scale.y);
	td.setFillColor(line);
	rt.draw(td);
}

void Default::draw_station(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) {
	sf::CircleShape s = _simple_station;
	sf::Vector2f center = position + sf::Vector2f(0.5, 0.5);
	s.setPosition(center.x * _scale.x, center.y * _scale.y);
	s.setFillColor(line);
	rt.draw(s);
}

void Default::draw_station_transfer(sf::RenderTexture& rt, sf::Vector2f const& position) {
	sf::CircleShape st1 = _transfer_station_bg;
	sf::CircleShape st2 = _transfer_station_fg;
	sf::Vector2f center = (position + sf::Vector2f(0.5, 0.5));
	st1.setPosition(center.x * _scale.x, center.y * _scale.y);
	st2.setPosition(center.x * _scale.x, center.y * _scale.y);
	rt.draw(st1);
	rt.draw(st2);
}

void Default::draw_station_large(sf::RenderTexture& rt, sf::Vector2f const& origin, sf::Vector2f const& destination) {
	sf::Vector2f size_squares = destination - origin;
	
	sf::Vector2f t_origin = sf::Vector2f(origin.x * _scale.x - ((_stop_radius - _scale.x / 2)), origin.y * _scale.y - ((_stop_radius - _scale.y / 2)));
	sf::Vector2f t_destination = sf::Vector2f(destination.x * _scale.x + ((_stop_radius - _scale.x / 2)), destination.y * _scale.y + ((_stop_radius - _scale.y / 2)));
	//sf::/*Rounded*/RectangleShape rrs_bg = sf::/*Rounded*/RectangleShape(size_px/*, _stop_radius*/);
	sf::RoundedRectangleShape rrs_fg = sf::RoundedRectangleShape(t_destination - t_origin, _stop_radius, 200);
	//rrs_bg.setPosition(origin.x * _scale.x, origin.y * _scale.y);
	//sf::Vector2f p2 = origin + ((destination - origin) * 0.1f);
	rrs_fg.setPosition(t_origin);
	rrs_fg.setOutlineColor(sf::Color::Black);
	rrs_fg.setOutlineThickness(0.1f * _stop_radius);
	
	//rrs_bg.setFillColor(sf::Color::Black);
	rrs_fg.setFillColor(sf::Color::White);
	
	//rt.draw(rrs_bg);
	rt.draw(rrs_fg);
}

void Default::draw_station_terminus(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) {
	draw_station_transfer(rt, position);
	draw_terminus_dot(rt, line, position);
}

void Default::draw_center_circle(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) {
	sf::CircleShape center_circle = _center_circle;
	center_circle.setFillColor(line);
	sf::Vector2f center = (position + sf::Vector2f(0.5, 0.5));
	center_circle.setPosition(center.x * _scale.x, center.y * _scale.y);
	rt.draw(center_circle);
}



}
