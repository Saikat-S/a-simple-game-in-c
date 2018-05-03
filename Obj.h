struct PLAYER{
    int x;
    int y;
    int lives;
    int speed;
    int boundx;
    int boundy;
    int score;
    int level;
    int Boost;
};

struct STONE{
    int x;
    int y;
    bool live;
    int speed;
    int boundx;
    int boundy;
};

struct Background{
	float x;
	float y;
	float velX;
	float velY;
	int dirX;
	int dirY;

	int width;
	int height;

	ALLEGRO_BITMAP *image;
};
