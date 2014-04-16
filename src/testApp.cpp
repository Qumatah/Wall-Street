#include "testApp.h"

#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>

//--------------------------------------------------------------
void testApp::setup(){
    //startup values
    ofBackground(255,255,255);
    dayornight = true;
    taxigo = true;
    minutes = 0;
    cloudx = 0;
    taxix = 1440;
    planex = -5000;
    sunvalue = 0;
    
    //set % increase and decrease amplitude, *100 for percentage
    amplitude = 0.05;
    
    //set minus alert %
    minusalert = 1.0;
    minusalertf = -minusalert;
    
    //set plus alrt %
    plusalert = 1.0;
	
	//load font
	text.loadFont("Lubalin Graph Bold.ttf", 14);

    // loading the images
    cloud.loadImage("cloud.png");
    cloud2.loadImage("cloud.png");
    background.loadImage("background.png");
    fire_left.loadImage("fire_left.png");
    fire_right.loadImage("fire_right.png");
    fire_left2.loadImage("fire_left2.png");
    fire_right2.loadImage("fire_right2.png");
    dust.loadImage("dust.png");
    taxi.loadImage("taxi.png");
    plane.loadImage("airplane.png");
    legenda.loadImage("legenda.png");
    
    //string stocks[] = {"AAPL","GOOG","YHOO","BIDU","YNDX","MSFT","BCOR","IACI","FB","EBAY"};
    //string stocks[] = {"RAND.AS","ASML.AS","DL.AS","INGA.AS","SBM.AS","AGN.AS","BOKA.AS","REN.AS","PHI.AS","HEIA.AS"};
    
    bool parsingInterface = webapp.open("http://luisterluister.pythonanywhere.com/json");
	
    // fill the data structure
	int spacing = ofGetWidth()/132;
	for (int i=0;i<10;i++){
		Tickers[i].symbol = webapp["results"][i]["symbol"].asString();
		std::string temp_base = webapp["results"][i]["baseline"].asString();
		if (temp_base.compare("") == 0){
			Tickers[i].baseline = 0;
		}
		else{
			Tickers[i].baseline = ::atof(temp_base.c_str());
		}
		Tickers[i].y = ofGetHeight()/2;
        Tickers[i].fire = FALSE;
        Tickers[i].demolition = FALSE;
        Tickers[i].taxi = FALSE;
		std::string s = ofToString(i+1);
		s = s + ".png";
		Tickers[i].image.loadImage(s);
		Tickers[i].x = spacing;
		spacing+= Tickers[i].image.getWidth()+ofGetWidth()/132;
		cout << "populated datastructure: " << Tickers[i].symbol << ":" << Tickers[i].baseline << endl;
	}
	std::string symbols = Tickers[0].symbol;
    
	for (int i = 1;i<10;i++){
		symbols+= "%2C"+Tickers[i].symbol;
	}
    url = "http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D'http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3D"+symbols+"%26f%3Dsl1d1t1c1ohgv%26e%3D.csv'%20and%20columns%3D'symbol%2Cprice%2Cdate%2Ctime%2Cchange%2Ccol1%2Chigh%2Clow%2Ccol2'&format=json&diagnostics=true&callback=";
	fetch(3);
    for (int i = 1;i<10;i++){
		if (Tickers[i].baseline == 0){
			Tickers[i].baseline = getPrice(stockdata,Tickers[i].symbol);
			}
		}
}

//--------------------------------------------------------------
void testApp::update(){
	symbols = Tickers[0].symbol;
	for (int i = 1;i<10;i++){
		symbols+= "%2C"+Tickers[i].symbol;
	}
	url = "http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D'http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3D"+symbols+"%26f%3Dsl1d1t1c1ohgv%26e%3D.csv'%20and%20columns%3D'symbol%2Cprice%2Cdate%2Ctime%2Cchange%2Ccol1%2Chigh%2Clow%2Ccol2'&format=json&diagnostics=true&callback=";
	
	if (ofGetMinutes() > minutes){
		minutes = (ofGetMinutes()+1)%60;
		fetch(3);
		//compare interface to data first
		for (int i=0;i<10;i++){
		std::string symbol = webapp["results"][i]["symbol"].asString();
		std::string temp_base = webapp["results"][i]["baseline"].asString();
		if (temp_base.compare("") != 0){
		Tickers[i].baseline = ::atof(temp_base.c_str());
		}
		if (symbol.compare(Tickers[i].symbol) != 0){
			Tickers[i].demolition = true;
			}	
		}
		for (int i=0;i<10;i++){
            newprice = getPrice(stockdata,Tickers[i].symbol);
            
            if (newprice > Tickers[i].price || newprice < Tickers[i].price){
                sunvalue++;
            }
            if(newprice == Tickers[i].price){
                sunvalue--;
            }
            Tickers[i].price = newprice;
            
			Tickers[i].y = getY(Tickers[i].price,Tickers[i].baseline,amplitude);
            Tickers[i].change = (Tickers[i].price - Tickers[i].baseline)/Tickers[i].baseline*100;
            totalchange = totalchange + Tickers[i].change;
            //creates a percentage number and multiply by half of height
            //Tickers[i].y = ofGetHeight()/2*(Tickers[i].change+1);
            cout << Tickers[i].symbol << " changed since start: " << (Tickers[i].change) << "%" << endl;
            cout << "Total change:" << totalchange << "%" << endl;
           
            //setting buildings on fire!
            if (Tickers[i].change<minusalertf){
                Tickers[i].fire=TRUE;
            }
            else if (Tickers[i].change>minusalertf){
                Tickers[i].fire=FALSE;
            }
            
            if(Tickers[i].change > amplitude/10){
                driveTaxi(i, Tickers[i].x, taxix, taxi);
            }
		}
    }
        if(sunvalue > 1){
            //make it sunny
            dayornight = true;
        }
        if(sunvalue == 0){
            //make it nighty
            dayornight = false;
        }
    
	for (int i=0;i<10;i++){
		if (Tickers[i].demolition){
			Tickers[i].y++;
			Tickers[i].x += sin(ofGetFrameNum());
			if (Tickers[i].y > ofGetHeight()){
				Tickers[i].demolition = false;
				Tickers[i].symbol = webapp["results"][i]["symbol"].asString();
                Tickers[i].y = getY(Tickers[i].price,Tickers[i].baseline,amplitude);
			}
		}
	}

    //conversion naar een stringeding
    totalchange = floor(totalchange *100)/100;
    stringstream stringeding2;
    stringeding2 << totalchange;
    stringeding = stringeding2.str();
    
}
//--------------------------------------------------------------
void testApp::fetch(int attempts){
	if (attempts == 0){
	cout << "timed out" << endl;
	return;
	}
	bool parsingSuccessful = stockdata.open(url);
	bool parsingInterface = webapp.open("http://luisterluister.pythonanywhere.com/json");
	if (parsingSuccessful && parsingInterface) {
		cout << stockdata.getRawString(TRUE) << endl;
		
		return;
	} else {
		cout  << "Failed to parse JSON, retrying..." << endl;
		cout  << "Attempts left: " << attempts << endl; 
		fetch(attempts-1);
	}
	
}
//------------------------------------------------------------
void testApp::draw(){
    
    //create the background
    if(dayornight == true){
        ofBackground(180, 210, 240);
    }
    if(dayornight == false){
        ofBackground(120, 120, 150);
    }
    //creating background skyline
    background.draw(0,0);
    
    //moving cloud animation
    if(cloudx>cloud.getWidth()*-1){
    cloud.draw(cloudx, ofGetHeight()/2-cloud.getHeight());
    cloud2.draw(cloudx+cloud.getWidth(), ofGetHeight()/2-cloud.getHeight());
    cloudx=cloudx-0.1;
    }
    
    if(cloudx==cloud.getWidth()*-1){
        cloudx=0;
    }
    
    // Display building array.
	for (int i=0;i<10;i++){
        //drawing the buildings
		Tickers[i].image.draw(Tickers[i].x,Tickers[i].y);
        text.drawString(Tickers[i].symbol.substr(0,Tickers[i].symbol.find(".")),Tickers[i].x+20,Tickers[i].y+50);
        
        //drawing fire
        if(Tickers[i].fire){
            fire_left.draw(Tickers[i].x-5+sin(ofGetFrameNum()/5)*-1,Tickers[i].y+30);
            fire_left2.draw(Tickers[i].x-5+sin(ofGetFrameNum()/10),Tickers[i].y+30);
            fire_left.draw(Tickers[i].x+30+sin(ofGetFrameNum()/5)*-1,Tickers[i].y+150);
            fire_left2.draw(Tickers[i].x+30+sin(ofGetFrameNum()/10),Tickers[i].y+150);
            fire_right.draw(Tickers[i].x+Tickers[i].image.getWidth()-30+sin(ofGetFrameNum()/5)*-1, Tickers[i].y+40);
            fire_right2.draw(Tickers[i].x+Tickers[i].image.getWidth()-30+sin(ofGetFrameNum()/10), Tickers[i].y+40);
        }
        
        //drawing dust clouds
        if(Tickers[i].demolition){
            for(int dusty=0; dusty<2; dusty++){
                dust.draw(Tickers[i].x-10, ofGetHeight()-38+sin(ofGetFrameNum())), 50;
            }
            if(int dusty = 2){
                dusty--;
            }
        }
    }
    //taxi
    if(taxix>taxi.getWidth()*-1){
        taxi.draw(taxix,ofGetHeight() - taxi.getHeight());
        taxix=taxix-2.5;
    }
    if(taxix<0-taxi.getWidth()){
        taxix=4000;
    }
    //plane
    if(planex < ofGetWidth() + plane.getWidth()){
        plane.draw(planex,200);
        planex=planex+3;
    }
    if(planex>ofGetWidth() + plane.getWidth()){
        planex=-5000;
    }
    
    //putting the total change on the plane
    ofSetColor(255,255,255);
    text.drawString(stringeding+"%", planex+40,240);
    //text.drawString(stringeding,200,200);
    
    //draw legenda on screen
    if(legendaOn == true){
        legenda.draw(0,0);
    }
    if(legendaOn == false){
        legenda.draw(1440,0);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key == '1'){
        ofSetFullscreen(TRUE);
    }
    if (key == '2'){
        ofSetFullscreen(FALSE);
    }
    if (key == ' '){
        fetch(1);
    }
    if (key == 'd' || key == 'D'){
        dayornight = true;
    }
    if (key == 'n' || key == 'N'){
        dayornight = false;
    }
    if(key == 'l'|| key == 'L'){
        legendaOn = 1;
    }
    if(key == 'o' || key == 'O'){
        legendaOn = 0;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	for (int i = 0;i<10;i++){
		if (x>Tickers[i].x && x<Tickers[i].x+Tickers[i].image.getWidth() && y>Tickers[i].y){
			cout << "you just clicked the HQ of " << Tickers[i].symbol << endl;
			if (!Tickers[i].demolition){
				//erase datastructure
                Tickers[i].demolition = true;
			}
			else {
				Tickers[i].field = true;
			}
			return; 
		}
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
//--------------------------------------------------------------

float testApp::getPrice(const ofxJSONElement& j,std::string s){
	//given a symbol, extracts price
	//from a Yahoo Finance JSON object
	int length = j["query"]["count"].asInt();
	for(int i=0; i<length; i++){
		std::string symbol  = j["query"]["results"]["row"][i]["symbol"].asString();
		if (symbol.compare(s) == 0){
		//convert json::value to string, then to float
		//it seems .asFloat() operator is broken :(
        return ::atof(j["query"]["results"]["row"][i]["price"].asString().c_str());
		}
    }
	return 0.0f;
}

//--------------------------------------------------------------

float testApp::getY(float price, float baseline,float amplitude){
	//calculates top left corner value based on
	//price compared to baseline
	float y = ofGetHeight()/2.0;
	if (price>baseline){
		y -= (price-baseline)/baseline*(1.0/amplitude)*ofGetHeight()/2;
		if (y<0){
			return 0.0f;
		}
		else {
			return y;
		}
	}
	else {
		y += (baseline-price)/baseline*(1.0/amplitude)*ofGetHeight()/2;
		if (y>ofGetHeight()){
			return ofGetHeight();
		}
		else {
			return y;
		}
	}
}

//---------------------------------------------------------------

void testApp::driveTaxi(int buildnr, float endpoint, float taxix, ofImage taxi){
    if(taxix > endpoint){
        taxi.draw(taxix,ofGetHeight()-40);
        taxix--;
    }
    if(taxix == endpoint){
        Tickers[buildnr].taxi==FALSE;
    }
    taxix=1600;
}
