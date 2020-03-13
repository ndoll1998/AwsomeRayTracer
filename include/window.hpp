#include "SDL2/SDL.h"

class Window{
    
    private:
    /* global window id */
    static unsigned int global_id;
    /* sdl members */
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    /* window members */
    unsigned int width; unsigned int height;
    const unsigned int id;

    public:
    /* constructors and destructor */
    Window(unsigned int w, unsigned int h);
    ~Window(void);

    /* public methods */
    void show(void);
    /* manipulate pixels */
    void* pixels(void) const;
    void display(void);
    /* getters */
    const unsigned int get_id(void) const { return this->id; }
    const unsigned int get_width(void) const { return this->width; }
    const unsigned int get_height(void) const { return this->height; }
};