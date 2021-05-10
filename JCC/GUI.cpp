
#include "compiler.h"

int Create_window(sf:: Music* music, sf:: Music* joke_music)
{
    sf::RenderWindow window(sf::VideoMode(1920, 1980), "JCC", sf::Style::Default);

    sf::Texture background;
    sf::Sprite  background_image;

    if (!background.loadFromFile("GUI/back.jpg"))
    {
        printf("ERROR: Couldn't display \"GUI/back.jpg\"\n");
        return 1;
    }

    background_image.setTexture(background);

    sf::Font font;
    font.loadFromFile("GUI/Minecraft.ttf");

    sf::Texture exit_button;
    sf::Sprite  exit_button_image;

    if (!exit_button.loadFromFile("GUI/buttonN.png"))
    {
        printf("ERROR: Couldn't display \"GUI/buttonN.png\"\n");
        return 1;
    }
    
    exit_button_image.setPosition(75.0f, 600.0f);
    exit_button_image.setScale(2.5f, 2.5f);
    exit_button_image.setTexture(exit_button);

    sf::Texture compile_button;
    sf::Sprite  compile_button_image;

    if (!compile_button.loadFromFile("GUI/buttonY.png"))
    {
        printf("ERROR: Couldn't display \"GUI/buttonY.png\"\n");
        return 1;
    }
    
    compile_button_image.setPosition(75.0f, 400.0f);
    compile_button_image.setScale(2.5f, 2.5f);
    compile_button_image.setTexture(compile_button);

    sf::Texture mute_button;
    sf::Sprite  mute_button_image;

    if (!mute_button.loadFromFile("GUI/mute.png"))
    {
        printf("ERROR: Couldn't display \"GUI/mute.png\"\n");
        return 1;
    }
    
    mute_button_image.setPosition(1700.0f, 50.0f);
    mute_button_image.setScale(0.8f, 0.8f);
    mute_button_image.setTexture(mute_button);
    mute_button_image.setColor(sf::Color(20, 250, 20));

    sf::Texture unmute_button;
    sf::Sprite  unmute_button_image;

    if (!unmute_button.loadFromFile("GUI/unmute.png"))
    {
        printf("ERROR: Couldn't display \"GUI/unmute.png\"\n");
        return 1;
    }
    
    unmute_button_image.setPosition(1700.0f, 50.0f);
    unmute_button_image.setScale(0.8f, 0.8f);
    unmute_button_image.setTexture(unmute_button);

    sf::Texture to_be_continued;
    sf::Sprite  to_be_continued_image;

    if (!to_be_continued.loadFromFile("GUI/to-be-continued.jpg"))
    {
        printf("ERROR: Couldn't display \"GUI/to-be-continued.jpg\"\n");
        return 1;
    }
    
    to_be_continued_image.setPosition(1980.0f, 50.0f);
    to_be_continued_image.setScale(1.0f, 1.0f);
    to_be_continued_image.setTexture(to_be_continued);

    bool music_on         = true;
    bool input_resolution = false;
    int  result           = -1;
    bool delay            = 0;
    bool continued        = 0;
   
    sf::String user_input;

    sf::Text compile_text;
    Set_text(&compile_text, font, sf::Color::Black, 60, 158.0f, 425.0f, "Compile");

    sf::Text exit_text;
    Set_text(&exit_text, font, sf::Color::Black, 60, 210.0f, 620.0f, "Exit");

    sf::Text file_text;
    Set_text(&file_text, font, sf::Color::Green, 60, 1200.0f, 360.0f, "Input name file:");
    
    sf::Text input_text;
    Set_text(&input_text, font, sf::Color::Green, 60, 1200.0f, 420.0f);
    
    sf::Text error_input_text;
    Set_text(&error_input_text, font, sf::Color::Red, 60, 1200.0f, 420.0f, "ERROR: file not found!");
    
    sf::Text patch_text;
    Set_text(&patch_text, font, sf::Color::Green, 60, 1200.0f, 420.0f, "ELF file was created!!!");
   
    while (window.isOpen())
    {
        sf::Event Event;

        while (window.pollEvent(Event))
        {
            switch (Event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::MouseMoved:
                    {
                        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                        sf::Vector2f mouse_pos_f(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y));
                        
                         if (exit_button_image.getGlobalBounds().contains(mouse_pos_f))
                        {
                            exit_button_image.setColor(sf::Color(250, 20, 20));
                            compile_button_image.setColor(sf::Color(255, 255, 255));
                            mute_button_image.setColor(sf::Color(255, 255, 255));
                            unmute_button_image.setColor(sf::Color(255, 255, 255));
                        }

                        else if (compile_button_image.getGlobalBounds().contains(mouse_pos_f))
                        {
                            exit_button_image.setColor(sf::Color(255, 255, 255));
                            compile_button_image.setColor(sf::Color(20, 250, 20));
                            unmute_button_image.setColor(sf::Color(255, 255, 255));
                            mute_button_image.setColor(sf::Color(255, 255, 255));
                        }

                        else if (music_on && unmute_button_image.getGlobalBounds().contains(mouse_pos_f))
                        {
                            exit_button_image.setColor(sf::Color(255, 255, 255));
                            compile_button_image.setColor(sf::Color(255, 255, 255));
                            unmute_button_image.setColor(sf::Color(120, 255, 255));
                            mute_button_image.setColor(sf::Color(255, 255, 255));
                        }
                        
                        else if (!music_on && mute_button_image.getGlobalBounds().contains(mouse_pos_f))
                        {
                            exit_button_image.setColor(sf::Color(255, 255, 255));
                            compile_button_image.setColor(sf::Color(255, 255, 255));
                            unmute_button_image.setColor(sf::Color(255, 255, 255));
                            mute_button_image.setColor(sf::Color(20, 250, 20));
                        }

                        else
                        {
                            exit_button_image.setColor(sf::Color(255, 255, 255));
                            compile_button_image.setColor(sf::Color(255, 255, 255));
                            mute_button_image.setColor(sf::Color(255, 255, 255));
                            unmute_button_image.setColor(sf::Color(255, 255, 255));
                        }
                    }
                    break;
                
                case sf::Event::MouseButtonPressed:
                {
                    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                    sf::Vector2f mouse_pos_f( static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y));
                    
                    if (exit_button_image.getGlobalBounds().contains(mouse_pos_f) && !continued)
                    { 
                        window.close();
                    }

                    if (to_be_continued_image.getGlobalBounds().contains(mouse_pos_f) && continued)
                    {
                        window.close();
                    }

                    if (compile_button_image.getGlobalBounds().contains(mouse_pos_f))
                    {
                        input_resolution = true;
                        result = -1;
                    }

                    if (music_on && unmute_button_image.getGlobalBounds().contains(mouse_pos_f))
                    {
                        music_on = false;
                        music->setVolume(0);
                    }

                    else if ((!music_on) && mute_button_image.getGlobalBounds().contains(mouse_pos_f))
                    {
                        music_on = true;
                        music->setVolume(50);
                    }

                }
                break;

                case sf::Event::KeyPressed:
                {
                    if (input_resolution)
                    {
                        if (Event.key.code == sf::Keyboard::Enter)
                        {
                            input_resolution = false;

                            result = Compile(user_input.toAnsiString().c_str()); 

                            user_input.clear();
                            input_text.setString(user_input);

                            if (result == 1) 
                            {
                                delay = 1;
                            }
                        }

                        else if (Event.key.code == sf::Keyboard::BackSpace)
                        {

                            if (user_input.getSize() != 0);
                            {
                                user_input.erase(user_input.getSize() - 1);
                                input_text.setString(user_input);
                            }
                        }
                          
                    }
                }
                break;

                case sf::Event::TextEntered:
                {
                    if (input_resolution && Event.text.unicode != '\b')
                    {
                        user_input.insert(user_input.getSize(), Event.text.unicode);
                        input_text.setString(user_input);
                    }  
                }
                break;

                default: break;
            }
        }

        window.clear();
        window.draw(background_image);

        if (!delay && !continued)
        {
            window.draw(exit_button_image);
            window.draw(exit_text);
        }

        if (input_resolution) 
        {   
            window.draw(file_text);
            window.draw(input_text);
        }
        
        switch (result)
        {
            case 0: 
            {
                window.draw(patch_text);
            }
            break;
            
            case -1:
            {
                window.draw(compile_button_image);
                window.draw(compile_text);
            }
            break;

            default: break;
        }

        if (music_on)
        {
            window.draw(unmute_button_image);
        }
        else
        {
            window.draw(mute_button_image);
        }

        if (continued == 1)
        {
            window.draw(to_be_continued_image);

            if (to_be_continued_image.getPosition().x != 0)
            {
                to_be_continued_image.setPosition(to_be_continued_image.getPosition().x - 10, to_be_continued_image.getPosition().y);
            }

            else 
            {
                window.draw(error_input_text);
            }
        }

        window.display();

        if (delay == 1) 
        {   
            music->stop();
            joke_music->play();

            delay = 0;
            for (size_t i = 0; i < 4500000000; ++i);
            continued = 1;
        }
    } 

    return 0;
}

void Set_text(sf::Text* text, const sf::Font& font, const sf::Color color, size_t font_size, float x, float y)
{
    text->setFont(font);
    text->setStyle(sf::Text::Bold);
    text->setFillColor(color);
    text->setCharacterSize(font_size);
    text->setPosition(x, y);
}

void Set_text(sf::Text* text, const sf::Font& font, const sf::Color color, size_t font_size, float x, float y, const char* str)
{
    text->setFont(font);
    text->setStyle(sf::Text::Bold);
    text->setString(str);
    text->setFillColor(color);
    text->setCharacterSize(font_size);
    text->setPosition(x, y);
}