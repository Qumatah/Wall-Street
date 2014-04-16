#pragma once

#include "ofMain.h"
#include "ofxJSONElement.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void fetch(int attempts);
        ofxJSONElement stockdata;
		ofxJSONElement webapp;
		ofTrueTypeFont text;
        std::string symbols;
        std::string url;
		int minutes;
		struct Ticker
		{
			std::string symbol;
			float y;
			float x;
			float baseline;
			float price;
            float change;
            Boolean fire;
            Boolean demolition;
            Boolean taxi;
			bool field;
			ofImage image;
		};
		Ticker Tickers[10];
    
        float getPrice(const ofxJSONElement& j, std::string s);
        float getY(float price, float baseline,float amplitude);
        void driveTaxi(int buildnr, float endpoint, float taxix, ofImage taxi);
    
        //images
        ofImage cloud;
        ofImage cloud2;
        ofImage dust;
        ofImage background;
        ofImage fire_left;
        ofImage fire_right;
        ofImage fire_left2;
        ofImage fire_right2;
        ofImage taxi;
        ofImage plane;
        ofImage legenda;
        
        float cloudx;
        float taxix;
        float planex;
        std::string stringeding;
        int buildnr;
        Boolean taxigo;
        Boolean legendaOn;
    
        //user input
        float amplitude;
        float minusalert;
        float minusalertf;
        float plusalert;
    
        //time and weather
        float newprice;
        float totalchange;
        Boolean dayornight;
        int sunvalue;
        int sunv;
};
