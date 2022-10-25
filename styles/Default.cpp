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
		
		_transfer_station.setRadius(_stop_radius);
		_transfer_station.setOrigin(_stop_radius, _stop_radius);
		_transfer_station.setFillColor(sf::Color::White);
		_transfer_station.setOutlineThickness(0.1f * _stop_radius);
		_transfer_station.setOutlineColor(sf::Color::Black);
		
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
	sf::RectangleShape* rect;
	switch(destination_id) {
	case 0:
	case 8:
		rect = &_rect_ed;
		break;
	case 1:
	case 7:
		rect = &_rect_h;
		break;
	case 2:
	case 6:
		rect = &_rect_od;
		break;
	case 3:
	case 5:
		rect = &_rect_v;
		break;
	default: // Should not happen
		return;
	}
	
	if(destination_id <= 3)
		rect->setRotation(rect->getRotation() + 180);
	
	rect->setPosition(center);
	rect->setFillColor(line);
	rt.draw(*rect);
	
	if(destination_id <= 3)
		rect->setRotation(rect->getRotation() - 180);
}

void Default::draw_terminus_dot(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) {
	sf::Vector2f center = position + sf::Vector2f(0.5, 0.5);
	_terminus_dot.setPosition(center.x * _scale.x, center.y * _scale.y);
	_terminus_dot.setFillColor(line);
	rt.draw(_terminus_dot);
}

void Default::draw_station(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) {
	sf::Vector2f center = position + sf::Vector2f(0.5, 0.5);
	_simple_station.setPosition(center.x * _scale.x, center.y * _scale.y);
	_simple_station.setFillColor(line);
	rt.draw(_simple_station);
}

void Default::draw_station_transfer(sf::RenderTexture& rt, sf::Vector2f const& position) {
	sf::Vector2f center = (position + sf::Vector2f(0.5, 0.5));
	_transfer_station.setPosition(center.x * _scale.x, center.y * _scale.y);
	rt.draw(_transfer_station);
}

void Default::draw_station_large(sf::RenderTexture& rt, sf::Vector2f const& origin, sf::Vector2f const& destination) {
	sf::Vector2f t_origin = sf::Vector2f(origin.x * _scale.x - ((_stop_radius - _scale.x / 2)), origin.y * _scale.y - ((_stop_radius - _scale.y / 2)));
	sf::Vector2f t_destination = sf::Vector2f(destination.x * _scale.x + ((_stop_radius - _scale.x / 2)), destination.y * _scale.y + ((_stop_radius - _scale.y / 2)));
	sf::RoundedRectangleShape rrs_fg = sf::RoundedRectangleShape(t_destination - t_origin, _stop_radius, 200);
	rrs_fg.setPosition(t_origin);
	rrs_fg.setOutlineColor(sf::Color::Black);
	rrs_fg.setOutlineThickness(0.1f * _stop_radius);
	
	rrs_fg.setFillColor(sf::Color::White);
	
	rt.draw(rrs_fg);
}

void Default::draw_station_terminus(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) {
	draw_station_transfer(rt, position);
	draw_terminus_dot(rt, line, position);
}

void Default::draw_center_circle(sf::RenderTexture& rt, sf::Color const& line, sf::Vector2f const& position) {
	_center_circle.setFillColor(line);
	sf::Vector2f center = (position + sf::Vector2f(0.5, 0.5));
	_center_circle.setPosition(center.x * _scale.x, center.y * _scale.y);
	rt.draw(_center_circle);
}



}
