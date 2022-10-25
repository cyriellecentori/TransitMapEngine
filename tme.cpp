#include "tme.hpp"
#include <fstream>
#include <iostream>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cmath>

#if defined(__linux__)
#include <sys/ioctl.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <iomanip>

#include "styles/Default.hpp"

int main(int argc, char** argv) {
	if(argc != 2) {
		std::cout << "This program takes one argument: the name of the yaml data file used to build the map. No more, no less." << std::endl;
		return 1;
	}

	TME::Main main{std::string(argv[1])};
	return 0;
}

std::string loading_bar(int percent, int size, char character) {
	std::string bar = "[";
	for(int i = 0; i < size; i++) {
		if((i/size) * 100 <= percent) {
			bar += character;
		} else {
			bar += " ";
		}
	}
	return bar + "]";
}

std::list<int> range(int i) {
	std::list<int> ret;
	for(int itor = 0; itor < i; itor++) {
		ret.push_back(itor);
	}
	return ret;
}

std::list<int> oddeven_sort(std::list<int> const& tab) {
	std::list<int> ntab;
	for(int t : tab) {
		if(t % 2 == 1) {
			ntab.push_front(t);
		} else {
			ntab.push_back(t);
		}
	}
	
	return ntab;
}

namespace TME {

Main::Main(std::string const& file)
	: data(YAML::LoadFile(file)) {
	
	std::map<std::string, YAML::Node> styles_yaml = data["styles"].as<std::map<std::string, YAML::Node> >();
	
	// Rounds to the nearest even number to have centers in a corner of a pixel
	scale = sf::Vector2f(((data["scale"][0].as<int>()) / 2) * 2, ((data["scale"][1].as<int>()) / 2) * 2);
	
	#if defined(__linux__)
	struct winsize w;
    	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int colnum = w.ws_col / 4;
	#elif defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	int colnum = (csbi.srWindow.Right - csbi.srWindow.Left + 1) / 4;
	#else
	int colnum = 10;
	#endif

	std::cout << "Loading lines map…" << std::endl;
	
	sf::Image img = sf::Image();
	img.loadFromFile(data["map"].as<std::string>());
	font.loadFromFile(data["font"]["path"].as<std::string>());

	std::cout << "File loaded, identifying lines and extracting map data…" << std::endl;
	
	for(unsigned int x = 0; x < img.getSize().x; x++) {
		map.push_back(std::vector<int>());
		for(unsigned int y = 0; y < img.getSize().y; y++) {
			const sf::Color& c = img.getPixel(x, y);
			
			int percent = round((((img.getSize().x * x - 1) + y) / (img.getSize().y * img.getSize().x)) * 100);
			std::cout << loading_bar(percent, colnum, '#') << " – " << std::setw(3) << percent << " %\r";
			
			int n = 0;
			if(c == sf::Color::Black) {
				n = -1;
			} else if(c != sf::Color::White) {
				if(!contains(lines, c)) {
					lines.push_back(c);
				}
				n = find(lines, c) + 1;
			}
			map[x].push_back(n);
		}
	}
	
	std::cout << loading_bar(100, colnum, '#') << " – 100 % – Done!" << std::endl;
	
	std::cout << lines.size() << " lines detected. Loading styles." << std::endl;
	
	for(std::pair<std::string, YAML::Node> style : styles_yaml) {
		std::string type = style.second["type"].as<std::string>();
		if(type == "default") {
			styles[style.first] = new Styles::Default(scale, style.second["stop size"].as<float>(), style.second["line width"].as<float>());
		}
		if(style.first != "default") { // style named "default" is the default for any line without a defined style
			for(std::string line : style.second["lines"].as<std::vector<std::string> >()) {
				line_styles[find(lines, sf::Color(std::stoi(line, 0, 16)))] = styles[style.first];
			}
		}
	}
	
	std::cout << "Styles loaded." << std::endl;
	
	
	sf::RenderTexture rt = sf::RenderTexture();	
	rt.create(scale.x * img.getSize().x, scale.y * img.getSize().y);
	
	std::vector<std::pair<int, int> > nb;
	nb.push_back(std::pair<int, int>(1, 3));
	nb.push_back(std::pair<int, int>(0, 2));
	nb.push_back(std::pair<int, int>(1, 5));
	nb.push_back(std::pair<int, int>(0, 6));
	nb.push_back(std::pair<int, int>(-1, -1));
	nb.push_back(std::pair<int, int>(2, 8));
	nb.push_back(std::pair<int, int>(3, 7));
	nb.push_back(std::pair<int, int>(6, 8));
	nb.push_back(std::pair<int, int>(5, 7));
	
	int ids[7] = {1, 3, 5, 7, 0, 2, 8};
	
	std::cout << "Generating map…" << std::endl;
	
	for(int x = 0; x < img.getSize().x; x++) {
		for(int y = 0; y < img.getSize().y; y++) {
			int percent = round((((img.getSize().x * x - 1) + y) / (img.getSize().y * img.getSize().x)) * 100);
			std::cout << loading_bar(percent, colnum, '#') << " – " << std::setw(3) << percent << " %\r";
			
			sf::Vector2f vec_pos = sf::Vector2f(x, y);
			int line = map[x][y];
			sf::Color line_color = lines[line - 1];
			Styles::Style* line_style = line_styles.contains(line - 1) ? line_styles[line - 1] : styles["default"];
			if(x > 0 && y > 0 && x < (img.getSize().x - 1) && y < (img.getSize().y - 1) && line != 0) {
				std::vector<int> coord_same;
				
				if(line != -1) {
					line_style->draw_center_circle(rt, line_color, vec_pos);
				}
				
				std::map<int, std::vector<int> > coorlock; // Locks possibilities so there is at least 90° between each line of a same color.
				
				for(int i = 0,c = ids[0]; i < 7; i++, c = ids[i]) {
					int s_x = c / 3 - 1, s_y = c % 3 - 1;
					for(int d : oddeven_sort(range(8 - c))) {
						int c_ = 8 - d;
						if(c_ != 4) { // Not center
							int s_x_ = c_ / 3 - 1, s_y_ = c_ % 3 - 1;
							
							// Might be too complicated for what it does, but I’m too lazy to check it
							if(	(
									(
										(map[x + s_x][y + s_y] == map[x + s_x_][y + s_y_]) && line == map[x + s_x_][y + s_y_]
									) || (
										(map[x + s_x][y + s_y] == -1 && map[x + s_x_][y + s_y_] == line) 
										|| 
										(map[x + s_x_][y + s_y_] == -1 && map[x + s_x][y + s_y] == line)
									)
								)
								&& 
								(coorlock.contains(line) ? 
									(!contains(coorlock[line], c) && !contains(coorlock[line], c_)) 
									: true)) { // Draw a segment if two are connected and they are of the same line or if one of the same line is connected with -1 and not too close from existing connection
								
								line_style->draw_segment(rt, line_color, vec_pos, c, c_);
								if(!coorlock.contains(line)) {
									coorlock[line] = std::vector<int>();
								}
								coorlock[line].push_back(nb[c].first);
								coorlock[line].push_back(nb[c].second);
								coorlock[line].push_back(nb[c_].first);
								coorlock[line].push_back(nb[c_].second);
							} else if(line == -1 && map[x + s_x][y + s_y] == map[x + s_x_][y + s_y_] && map[x + s_x][y + s_y] != 0 && (coorlock.contains(line) ? (!contains(coorlock[map[x + s_x][y + s_y]], c) && !contains(coorlock[map[x + s_x_][y + s_y_]], c_) ) : true) ) { // Connect everything if line -1 and not too close to existing connection
								Styles::Style* line_style_2 = line_styles.contains(map[x + s_x][y + s_y] - 1) ? line_styles[map[x + s_x][y + s_y] - 1] : styles["default"];
								line_style_2->draw_segment(rt, lines[map[x + s_x][y + s_y]], vec_pos, c, c_);
								if(!coorlock.contains(map[x + s_x][y + s_y])) {
									coorlock[map[x + s_x][y + s_y]] = std::vector<int>();
								}
								
								coorlock[map[x + s_x][y + s_y]].push_back(nb[c].first);
								coorlock[map[x + s_x][y + s_y]].push_back(nb[c].second);
								coorlock[map[x + s_x][y + s_y]].push_back(nb[c_].first);
								coorlock[map[x + s_x][y + s_y]].push_back(nb[c_].second);
							}
						}
					}
					
					if(map[x + s_x][y + s_y] == line || map[x + s_x][y + s_y] == -1) { // Lists all adjacent tiles of the same line for when there is only one
						coord_same.push_back(c);
					}
				}
				
				if(coord_same.size() == 1) {
					line_style->draw_half_segment(rt, line_color, vec_pos, coord_same[0]);
				}
			}
		}
	}
	
	std::cout << loading_bar(100, colnum, '#') << " – 100 % – Done!" << std::endl;
	
	std::cout << "Adding stops…" << std::endl;
	
	YAML::Node stops = data["stops"];
	int itor = 0;
	
	for(YAML::Node stop : stops) {
		int percent = round((itor / stops.size()) * 100);
		itor++;
		std::cout << loading_bar(percent, colnum, '#') << " – " << std::setw(3) << percent << " %\r";
		
		std::string stop_type = stop["type"].as<std::string>();
		sf::Vector2f stop_pos = sf::Vector2f(stop["position"][0].as<int>(), stop["position"][1].as<int>());
		int stop_line = map[stop_pos.x][stop_pos.y];
		sf::Color const& line_color = lines[stop_line - 1];
		Styles::Style* line_style = line_styles.contains(stop_line - 1) ? line_styles[stop_line - 1] : styles["default"];
		if(stop_type == "terminus") {
			line_style->draw_station_terminus(rt, line_color, stop_pos);
		} else if(stop_type == "transfer") {
			line_style->draw_station_transfer(rt, stop_pos);
		} else if(stop_type == "large") {
			line_style->draw_station_large(rt, stop_pos, sf::Vector2f(stop["position"][2].as<int>(), stop["position"][3].as<int>()));
		} else if(stop_type == "terminus dot") {
			line_style->draw_terminus_dot(rt, line_color, stop_pos);
		} else { // "single" by default
			line_style->draw_station(rt, line_color, stop_pos);
		}
		
		int font_size = data["font"]["size"].as<int>();
		sf::Color text_color = sf::Color(data["font"]["color"][0].as<int>(), data["font"]["color"][1].as<int>(), data["font"]["color"][2].as<int>());
		
		
		sf::Text stop_name = sf::Text("", this->font, font_size);
		if(stop_type != "terminus dot") {
			std::string str = stop["name"].as<std::string>();
			stop_name.setString(sf::String::fromUtf8(str.begin(), str.end()));
			stop_name.setFillColor(text_color);
			stop_name.setOutlineColor(text_color);
			
			sf::Vector2f origin = sf::Vector2f(0, 0);
			sf::Vector2f name_placement = sf::Vector2f(stop["name-placement"][0].as<float>(), stop["name-placement"][1].as<float>());
			if(stop["name-origin"]) {
				origin.x = stop["name-origin"][0].as<float>() * stop_name.getLocalBounds().width;
				origin.y = stop["name-origin"][1].as<float>() * stop_name.getLocalBounds().height;
			} else {
				if(name_placement.x < 0) {
				origin.x = stop_name.getLocalBounds().width + scale.x / 5.0f;
				} else if(name_placement.x > 0) {
					origin.x = - scale.x / 5.0f;
				} else {
					origin.x = stop_name.getLocalBounds().width / 2;
				}
				
				if(name_placement.y < 0) {
					origin.y = stop_name.getLocalBounds().height + scale.y / 5.0f;
				} else if(name_placement.y > 0) {
					origin.y = - scale.y / 10.0f;
				} else {
					origin.y = 2*font_size / 3;
				}
			}
			
			if(stop["name-rotation"]) {
				stop_name.setRotation(stop["name-rotation"].as<float>());
			}
			
			stop_name.setOrigin(origin);
			stop_name.setPosition((stop_pos.x + 0.5 + name_placement.x) * scale.x, (stop_pos.y + 0.5 + name_placement.y) * scale.y);
			rt.draw(stop_name);
			
		}
		
	}
	
	std::cout << loading_bar(100, colnum, '#') << " – 100 % – Done!" << std::endl;
	std::cout << "Map generated! Saving…" << std::endl;
	
	rt.display();
	
	rt.getTexture().copyToImage().saveToFile(data["output"].as<std::string>());
	
	std::cout << "Saved!" << std::endl;
	
	for(auto style : styles) {
		delete(style.second);
	}
	
}

}
