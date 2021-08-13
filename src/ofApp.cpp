#include "ofApp.h"

void ofApp::setup() 
{
	// initialize vectors
	offsetX.resize(NUM_OF_POINTS);
	offsetY.resize(NUM_OF_POINTS);

	isConnected.resize(NUM_OF_POINTS, false);

	bandXpos.resize(NUM_OF_BANDS);

	// get audio files from audio folder
	getAudioFiles();

	// get image files from images folder
	getImageFiles();

	// play first audio if audio files exists
	if (audioFiles.size() > 0) {
		soundPlayer.load(audioFiles[audioIndex]);
		soundPlayer.play();
	}

	// load background image if image files exist
	if (imageFiles.size() > 0) {
		backgroundImage.load(imageFiles[imageIndex]);
	}

	// initialize coordinate and color of points
	for (size_t i = 0; i < NUM_OF_POINTS; i++) {
		offsetX[i] = ofRandom(0, 1000);
		offsetY[i] = ofRandom(0, 1000);

		pointColors.push_back(*(new Color(0, 0, 0)));
	}

	// initialize spectrum and band position
	for (size_t i = 0; i < NUM_OF_BANDS; i++) {
		soundSpectrums[i] = 0;
		bandXpos[i] = (BAND_WIDTH + 1) * i;
	}

	// set loop mode for audio play
	soundPlayer.setLoop(true);

	// disable to erase background automatically
	ofSetBackgroundAuto(isShowShadowing);

}

// get audio files in songs folder
void ofApp::getAudioFiles()
{
	// open directory
	string path = "../../songs/";
	ofDirectory dir(path);

	// set filter
	dir.allowExt("wav");
	dir.allowExt("mp3");

	// get file list
	dir.listDir();

	// browse files in folder
	for (int i = 0; i < dir.size(); i++) {
		audioFiles.push_back(dir.getPath(i));
	}
}

void ofApp::getImageFiles()
{
	// open dirctory
	string path = "../../images/";
	ofDirectory dir(path);

	// set filter
	dir.allowExt("bmp");
	dir.allowExt("jpg");
	dir.allowExt("png");

	// get file list
	dir.listDir();

	// browse files in folder
	for (int i = 0; i < dir.size(); i++) {
		imageFiles.push_back(dir.getPath(i));
	}
}

// update sounds spectrum and points and bars position
void ofApp::update() 
{
	// update sound
	ofSoundUpdate();

	// update pints and bars position
	updatePoints();
	updateBars();
}

// Randomizes the color of a certain point and its line color once that point has been disconnected
void ofApp::updateColors() 
{
	// for connected points
	for (size_t i = 0; i < isConnected.size(); i++) {
		if (!isConnected[i]) {
			// 10 total options for coloring
			int options = ofRandom(10);

			if (options == 1) {				// 10% chance that point/line is colored with some mixture of green and blue
				pointColors[i].r = 50;
				pointColors[i].g = ofRandom(100, 150);
				pointColors[i].b = ofRandom(50, 125);
			} 
			else if (options == 2) {		// 10% chance that point/line is colored with some mixture of red and blue
				pointColors[i].r = ofRandom(125, 175);
				pointColors[i].g = 50;
				pointColors[i].b = ofRandom(50, 125);
			} 
			else {							// 80% chance that the point/line is colored black
				pointColors[i].r = 0;
				pointColors[i].g = 0;
				pointColors[i].b = 0;
			}
		}
	}
}

// Updates the location of the point on the screen
void ofApp::updatePoints() 
{
	double elapsedTime = ofGetElapsedTimef();

	// Change in time since last update()
	double dt = elapsedTime - currentTime;
	currentTime = elapsedTime;

	// get max frequency value
	float maxFrequency = getMaxFrequency();

	// update point's location
	for (size_t i = 0; i < NUM_OF_POINTS; i++) {
		// Get distance traveled. Speed of point depends on the highest frequency at the current time
		offsetX[i] += INIT_VELOCITY_OF_POINT * dt * maxFrequency * speedMultiplier;
		offsetY[i] += INIT_VELOCITY_OF_POINT * dt * maxFrequency * speedMultiplier;
	
		// Update position using Perlin Noise 
		// https://en.wikipedia.org/wiki/Perlin_noise
		points[i].x = ofSignedNoise(offsetX[i]) * RADIUS_OF_CLOUD;
		points[i].y = ofSignedNoise(offsetY[i]) * RADIUS_OF_CLOUD - 50;
	}
}

// Finds highest frequency value in sound spectrum
float ofApp::getMaxFrequency()
{
	float max = 0.0F;

	for (size_t i = 0; i < NUM_OF_BANDS; i++) {
		if (max < soundSpectrums[i]) {
			max = soundSpectrums[i];
		}
	}

	return max;
}

// Updates the audio reactive bands that are displayed on the screen
void ofApp::updateBars() 
{
	// Audio reactive shape
	float *val = ofSoundGetSpectrum(NUM_OF_BANDS);

	for (int i = 0; i < NUM_OF_BANDS; i++) {
		soundSpectrums[i] *= SOUND_SPECTRUM_SMOPTHNESS;
		soundSpectrums[i] = max(soundSpectrums[i], val[i]);
		bandXpos[i] += 1;

		// Once band goes out of screen it will loop around
		if (bandXpos[i] > ofGetScreenWidth()) { 
			bandXpos[i] = -BAND_WIDTH;
		}
	}
}

void ofApp::draw() 
{
	if (isShowShadowing) {
		ofSetColor(0, 0, 0, 10);		// 10 percent
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	}

	// calculate background color
	if (isShowBands || isShowRainbows) {
		if (isReachedMaxBandHeight && backgroundColor <= MAX_OF_COLOR_VALUE - BACKGROUND_CHANGE_RATE1) {
			backgroundColor += BACKGROUND_CHANGE_RATE1;
		}
		else if (backgroundColor >= 0) {
			backgroundColor -= BACKGROUND_CHANGE_RATE1;
		}
	}
	else {
		if (backgroundColorDirection) {
			if (backgroundColor <= MAX_OF_COLOR_VALUE - BACKGROUND_CHANGE_RATE2) {
				backgroundColor += BACKGROUND_CHANGE_RATE2;
			}
			else {
				backgroundColorDirection = false;
			}
		}
		else {
			if (backgroundColor >= 0) {
				backgroundColor -= BACKGROUND_CHANGE_RATE2;
			}
			else {
				backgroundColorDirection = true;
			}
		}
	}

	// draw background image
	if (isShowBackgroundImage) {
		ofSetColor(backgroundColor);
		backgroundImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
	else {
		ofSetColor(backgroundColor);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	}
	
	// draw top/bottom bands
	if (isShowBands) {
		drawBands();
	}

	// draw points
	if (isShowPoints) {
		drawPoints();
	}

	if (isShowRainbows) {
		drawRainbows();
	}
}

void ofApp::drawBands() 
{
	isReachedMaxBandHeight = false;

	for (int i = 0; i < NUM_OF_BANDS; i++) {
		// Band height and hue changes depending on frequencies
		float bandHeight = soundSpectrums[i] * MAX_OF_BAND_HEIGHT;
		float bandRedHue = soundSpectrums[i] * MAX_OF_COLOR_VALUE + 100;

		ofSetColor(bandRedHue, 0 , 25);

		float bandHeightThreshold = MAX_OF_BAND_HEIGHT - MIN_OF_BAND_HEIGHT;

		// check pick value
		if (bandHeight >= bandHeightThreshold) {
			isReachedMaxBandHeight = true;
			ofSetColor(25, 175, 150);
		}

		// draw top bands
		ofDrawRectangle(ofGetWidth() - bandXpos[i], 0, BAND_WIDTH, (bandHeight * .6) + MIN_OF_BAND_HEIGHT);

		// draw bottom bands
		ofDrawRectangle(bandXpos[i], ofGetHeight(), BAND_WIDTH, -(bandHeight + MIN_OF_BAND_HEIGHT));
	}
}

// draw center bands
void ofApp::drawRainbows()
{
	ofColor drawColor;

	int length = 400;
	int barWidth = 40;

	for (int i = 0; i < NUM_OF_BANDS; i++) {
		// Band height and hue changes depending on frequencies
		float bandHeight = soundSpectrums[i] * MAX_OF_BAND_HEIGHT;
		float bandRedHue = soundSpectrums[i] * MAX_OF_COLOR_VALUE + 100;

		ofSetColor(bandRedHue, 0, 25);

		float bandHeightThreshold = MAX_OF_BAND_HEIGHT - MIN_OF_BAND_HEIGHT;

		// check pick value
		if (bandHeight >= bandHeightThreshold) {
			isReachedMaxBandHeight = true;
		}

		ofPushMatrix();
		{
			// move to center and rotate
			ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
			ofRotateDeg(360.0 / NUM_OF_BANDS * i);

			// make color
			drawColor.setHsb(255 / NUM_OF_BANDS * i, 255, 255, 50);

			// draw bands
			ofSetColor(drawColor);
			ofDrawRectangle(100, -barWidth / 2, soundSpectrums[i] * length, barWidth / 2);
		}
		ofPopMatrix();
	}
}

void ofApp::drawPoints() 
{
	if (isReachedMaxBandHeight) {
		bassBoost();
	} 
	else {
		clearBoost();
	}
	
	// Center points
	ofPushMatrix();
	{
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

		// Set color of points and create circles at those points
		for (size_t i = 0; i < NUM_OF_POINTS; i++) {
			ofSetColor(0);
			ofDrawCircle(points[i], pointRadius + 1);
			ofSetColor(150, 0, 50);
			ofDrawCircle(points[i], pointRadius);
		}

		connectPoints();
	}
	ofPopMatrix();
}

//Settings for when band reaches max height
void ofApp::bassBoost()
{
	pointRadius = 6;
	lineWidth = 4;
	speedMultiplier = 1.1;
}

//Normal Settings
void ofApp::clearBoost()
{
	pointRadius = 3;
	lineWidth = 2;
	speedMultiplier = .8;
}

// Connects points within a certain distance of each other
void ofApp::connectPoints() 
{
	for (size_t i = 0; i < NUM_OF_POINTS; i++) {
		isConnected[i] = false;

		for (size_t j = i + 1; j < NUM_OF_POINTS; j++) {
			// calculate distance
			double dist = ofDist(points[i].x, points[i].y, points[j].x, points[j].y);

			if (dist < DISTANCE_THRESHOULD) {
				isConnected[i] = true;

				// draw connection line
				ofSetColor(pointColors[i].r, pointColors[i].g, pointColors[i].b);
				ofSetLineWidth(lineWidth);
				ofDrawLine(points[i], points[j]);
			}
		}
	}

	updateColors();
}

void ofApp::keyPressed(int key)
{
	switch (key) {
		// background image on/off
		case '1':
			isShowBackgroundImage = !isShowBackgroundImage;
			break;

		// points on/off
		case '2':
			isShowPoints = !isShowPoints;

			if (isShowPoints) {
				isShowRainbows = false;
			}

			break;

		// top/bottom bands on/off
		case '3':
			isShowBands = !isShowBands;
			break;

		// change audio
		case '4':
			audioIndex++;

			if (audioIndex >= audioFiles.size()) {
				audioIndex = 0;
			}

			if (soundPlayer.isPlaying()) {
				soundPlayer.stop();
			}

			if (soundPlayer.isLoaded()) {
				soundPlayer.unload();
			}

			soundPlayer.load(audioFiles[audioIndex]);

			if (soundPlayer.isLoaded()) {
				soundPlayer.play();
			}

			break;

		// change background
		case '5':
			imageIndex++;

			if (imageIndex >= imageFiles.size()) {
				imageIndex = 0;
			}

			backgroundImage.clear();
			backgroundImage.load(imageFiles[imageIndex]);
			break;
		
		// rainbows image on/off
		case '6':
			/*
			isShowRainbows = !isShowRainbows;

			if (isShowRainbows) {
				isShowPoints = false;
			}
			*/
			break;

		// tracing on/off
		case '7':
			isShowShadowing = !isShowShadowing;

			// disable to erase background automatically
			ofSetBackgroundAuto(isShowShadowing);
			break;
	}
}

void ofApp::keycodePressed(ofKeyEventArgs& e) 
{
	cout << "KEY : " << e.key << endl;
	cout << "KEYCODE : " << e.keycode << endl;
}

