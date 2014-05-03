#ifndef _menu_hpp_
#define _menu_hpp_

enum class menu_choice {
    undefined,
    select_maze,
    next_maze,
    previous_maze,
    exit
};

void menu(sf::RenderWindow& window, sf::Font& font, sf::Text& text);

#endif