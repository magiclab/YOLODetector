#include "ofApp.h"

void ofApp::setup() 
{
	ofBackground(0);
	ofSetVerticalSync(true);

	thresh = 0.25;
	threshInc = 0.05;

	int camId = 0;
	ofVec2f camRes(320, 240);

	//std::string cfgfile = ofToDataPath( "cfg/yolo9000.cfg" );
	//std::string weightfile = ofToDataPath( "yolo9000.weights" );
	//std::string namesfile = ofToDataPath( "cfg/9k.names" );
	//darknet.init(cfgfile, weightfile, namesfile);
    
	std::string datacfg = ofToDataPath("cfg/coco.data");
	std::string cfgfile = ofToDataPath("cfg/tiny-yolo.cfg");
	std::string weightfile = ofToDataPath("tiny-yolo.weights");
	std::string nameslist = ofToDataPath("cfg/coco.names");

	//std::string datacfg = ofToDataPath("cfg/voc.data");
	//std::string cfgfile = ofToDataPath("cfg/tiny-yolo-voc.cfg");
	//std::string weightfile = ofToDataPath("tiny-yolo-voc.weights");
	//std::string nameslist = ofToDataPath("cfg/voc.names");
	//darknet.init(cfgfile, weightfile, datacfg, nameslist);

	ofxXmlSettings xml;
	if (xml.load("xml/setup.xml")) {
		if (xml.tagExists("setup")) {
			xml.pushTag("setup");
			if (xml.tagExists("camera")) {
				xml.pushTag("camera");
				camId = xml.getValue("dev_id",camId);
				camRes.x = xml.getValue("width",camRes.x);
				camRes.y = xml.getValue("height",camRes.y);
				xml.popTag();
			}
			if (xml.tagExists("yolo")) {
				xml.pushTag("yolo");
				datacfg = ofToDataPath(xml.getValue("data", "cfg/coco.data"));
				cfgfile = ofToDataPath(xml.getValue("cfg", "cfg/tiny-yolo.cfg"));
				weightfile = ofToDataPath(xml.getValue("weights", "tiny-yolo.weights"));
				nameslist = ofToDataPath(xml.getValue("names", "cfg/coco.names"));
				xml.popTag();
			}
			xml.popTag();
		}
		else {
			ofLogError("ofApp::setup") << " malformed setup.xml" << endl;
		}
	}
	else
	{
		ofLogError("ofApp::setup") << " cannot load setup.xml" << endl;
	}

	darknet.init(cfgfile, weightfile, nameslist);

	video.setDeviceID( camId );
	video.setDesiredFrameRate( 30 );
	video.initGrabber( camRes.x, camRes.y );
	ofSetWindowShape(camRes.x, camRes.y);
}

void ofApp::update()
{
	video.update();
}

void ofApp::draw()
{
    // detected objects with confidence < threshold are omitted
	//float thresh = ofMap( ofGetMouseX(), 0, ofGetWidth(), 0, 1 );

    // if a detected object overlaps >maxOverlap with another detected
    // object with a higher confidence, it gets omitted
    float maxOverlap = 0.25;
    
	ofSetColor( 255 );
	video.draw( 0, 0 );

	if( video.isFrameNew() ) {
		std::vector< detected_object > detections = darknet.yolo( video.getPixels(), thresh, maxOverlap );

		ofNoFill();	
		for( detected_object d : detections )
		{
			ofSetColor( d.color );
			glLineWidth( ofMap( d.probability, 0, 1, 0, 8 ) );
			ofNoFill();
			ofDrawRectangle( d.rect );
			//string s = d.label + ": " + ofToString(d.probability);
			string s;
			//if (d.label == "person") {
			//	s = "coder [naus3a]:";
			//}
			//else if(d.label=="cat"){
			//	s = "cat [kirk]";
			//}
			//else {
				s = d.label;
			//}
			s += ": "+ofToString(d.probability);
			ofDrawBitmapStringHighlight( s, d.rect.x, d.rect.y + 20 );
			cout << s << endl;
            
            // optionally, you can grab the 1024-length feature vector associated
            // with each detected object
            //vector<float> & features = d.features;
		}
	}

	ofPushStyle();
	ofSetColor(ofColor::white);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate(),2),20,20);
	ofDrawBitmapString("Confidence: "+ofToString(thresh,2)+" (up/down to adjust)",20,40);
	ofPopStyle();
}

void ofApp::keyPressed(int key) {
	switch (key)
	{
	case OF_KEY_UP:
		thresh += threshInc;
		if (thresh > 1.0)thresh = 1.0;
		break;
	case OF_KEY_DOWN:
		thresh -= threshInc;
		if (thresh < 0.0)thresh = 0.0;
		break;
	default:
		break;
	}
}