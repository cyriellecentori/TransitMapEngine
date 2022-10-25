#pragma once

#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Font.hpp>

#include "styles/Style.hpp"

int main(int argc, char** argv);

template<class T>
bool contains(std::vector<T> const& v, T const& o) {
	for(T e : v) {
		if(e == o) {
			return true;
		}
	}
	return false;
}

template<class T>
int find(std::vector<T> const& v, T const& o) {
	for(int i = 0; i < v.size(); i++) {
		if(v[i] == o) {
			return i;
		}
	}
	return -1;
}

std::string loading_bar(int percent, int size, char character);

namespace TME {

class Main {
	public:
		Main(std::string const& file);
		
	private:
		const YAML::Node data;
		std::vector<std::vector<int> > map = std::vector<std::vector<int> >();
		std::vector<sf::Color> lines = std::vector<sf::Color>(); 
		sf::Font font;
		
		sf::Vector2f scale;
		
		std::map<std::string, Styles::Style*> styles;
		std::map<int, Styles::Style*> line_styles;
		
};

}
