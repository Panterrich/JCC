#include "GUI.h"

int main()
{
    sf::Music music;
    sf::Music joke_music;

    if (!music.openFromFile("libr/GUI/jojo.ogg"))
    {
        printf("ERROR: Couldn't play \"libr/GUI/jojo.ogg\"\n");
        return 1;
    }

    if (!joke_music.openFromFile("libr/GUI/to_be_continued.ogg"))
    {
        printf("ERROR: Couldn't play \"libr/GUI/to_be_continued.ogg\"\n");
        return 1;
    }
        
    music.setLoop(true);
    music.play();
    music.setVolume(50);

    Create_window(&music, &joke_music);
    
    music.stop();

    return 0;
}
