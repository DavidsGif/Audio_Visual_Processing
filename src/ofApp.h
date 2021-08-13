#pragma once

#include "ofMain.h"

#define NUM_OF_POINTS				550
#define NUM_OF_BANDS				250
#define	MAX_OF_COLOR_VALUE			255
#define BACKGROUND_CHANGE_RATE1		5
#define BACKGROUND_CHANGE_RATE2		2
#define INIT_VELOCITY_OF_POINT		0.5
#define RADIUS_OF_CLOUD				625
#define DISTANCE_THRESHOULD			50

#define	BAND_WIDTH					11
#define MAX_OF_BAND_HEIGHT			300
#define MIN_OF_BAND_HEIGHT			25

#define SOUND_SPECTRUM_SMOPTHNESS	0.92


class ofApp : public ofBaseApp 
{
private:
	struct Color {
		Color(int red, int green, int blue) : r(red), g(green), b(blue){};

		int r;
		int g;
		int b;
	};

private:
	vector<Color>	pointColors;
	ofSoundPlayer	soundPlayer;
	ofImage			backgroundImage;

	int				backgroundColor = 200;
	bool			backgroundColorDirection = true;

	double			currentTime = 0;

	ofVec2f			points[NUM_OF_POINTS];

	vector<double>	offsetX;
	vector<double>	offsetY;
	
	float			pointRadius = 3;
	float			speedMultiplier = 1.1;

	float			lineWidth = 2;

	vector<bool>	isConnected;

	vector<int>		bandXpos;

	bool			isReachedMaxBandHeight = false;
	float			soundSpectrums[NUM_OF_BANDS];

	vector<string>	audioFiles;
	vector<string>	imageFiles;

	int				audioIndex = 0;
	int				imageIndex = 0;

	bool			isShowBackgroundImage = true;
	bool			isShowPoints = true;
	bool			isShowBands = true;
	bool			isShowRainbows = false;

	bool			isShowShadowing = false;

private:
	void	updatePoints();
	void	connectPoints();
	void	updateColors();
	void	updateBars();
	void	drawPoints();
	void	drawBands();
	void	drawRainbows();

	void	getAudioFiles();
	void	getImageFiles();

	float	getMaxFrequency();
	void	bassBoost();
	void	clearBoost();

public:
	void	setup();
	void	update();
	void	draw();
	void	keyPressed(int key);
	void	keycodePressed(ofKeyEventArgs& e);

};
