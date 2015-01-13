#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    
    /* IF SERVER?
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = 6666;
    options.bUseSSL = false;
    socket.setup(options);
    socket.addListener(this);
     */
    
    ofxLibwebsockets::ClientOptions options = ofxLibwebsockets::defaultClientOptions();
    options.port = 14949;
    options.bUseSSL = false;
    options.host = "127.0.0.1";
    socket.connect(options);
    socket.addListener(this);
    
    retryAfter = 100;
    
    startTime = ofGetElapsedTimeMillis();
    
    ofSetLogLevel("ofxSceneManager", OF_LOG_VERBOSE);
    sm.setup();
    twinkle = (twinkleScene*)sm.add(new twinkleScene() );
    noPlace = (NoPlaceScene*)sm.add(new NoPlaceScene());
    sm.add(new SyncScene(&localTime));
    sm.gotoScene("NoPlace");

    ///// Scenes n control panel
    sceneNames.push_back("scene 0");
    sceneNames.push_back("scene 1");
    sceneNames.push_back("scene 2");
    sceneNames.push_back("scene 3");
    sceneNames.push_back("sync");
    sceneNames.push_back("no place");
    sceneNames.push_back("twinkle");


    
    setDrawControlPanel(true);
    
    controlPanel.addPanel("Scene Control", 1);
    controlPanel.setWhichPanel("Scene Control");
    controlPanel.addTextDropDown("Scene", "scene", 0, sceneNames);
    controlPanel.setupEvents();
    controlPanel.enableEvents();
    ofAddListener(controlPanel.guiEvent, this, &ofApp::onGuiEvent);
    bDebug = true;

}

//--------------------------------------------------------------
void ofApp::update(){
    localTime = ofGetElapsedTimeMillis()-startTime;
    
    if (!socket.isConnected() && retryCounter>retryAfter) {
        socket.connect("127.0.0.1");
        retryCounter=0;
    } else if (!socket.isConnected() && retryCounter < retryAfter){
        retryCounter++;
    }
    ofSetWindowTitle("ozHost | fps: " + ofToString(ofGetFrameRate()) );
    
    
    sm.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    //ofBackground(0);
    popTransforms();
    sm.draw();
    
    //if(bDebug) drawControlPanel();
}

void ofApp::onConnect(ofxLibwebsockets::Event & e){
    cout<< e.conn.getClientName()<<" connected!"<<endl;
    retryCounter =0;
    
}

void ofApp::onMessage(ofxLibwebsockets::Event & e){
    cout << "message: " << e.message <<endl;
    vector<string> msg = ofSplitString( e.message , " ");
    if (msg[0] == "/time"){
        remoteTime = ofToInt(msg[1]);
        if( abs( localTime-remoteTime ) > 10 ){
            resync(remoteTime);
        }
    } else if (msg[0] == "/trigger"){
        twinkle->triggerPulse(ofToInt(msg[1]));
    } else if (msg[0] =="/welcome"){
        socket.send("/imHost");

    } else if (msg[0] == "/sceneChange" ){
        switch (ofToInt(msg[1])){
            case 4:
                sm.gotoScene("sync");
                break;
            case 5:
                sm.gotoScene("NoPlace");
                break;
            case 6:
                sm.gotoScene("twinkle");
                break;
            default:
                cout<<"scene changed to empty scene: "<<msg[1]<<endl;
        }
    } else if(msg[0] == "/home"){
        noPlace->addHome(msg[1]);
    }
    
}

void ofApp::onGuiEvent(guiCallbackData & d){
    if(d.getDisplayName() == "Scene"){
        socket.send("/setScene " +d.getString(0));
        
    }
}

void ofApp::onClose(ofxLibwebsockets::Event & e){
    cout<<"disconnected!"<<endl;
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
    if(key>48 && key< 58){
        /*
        int scene = key-48;
        socket.send("/setScene "+ ofToString(scene));
        */
    } else{
        switch(key){
            case('r'):
                resync(remoteTime);
                break;
            case('p'):
                socket.send("/ping");
                break;
            case('t'):
                try{
                    twinkle->triggerPulse(ofRandom(0,2000));
                } catch(...){
                    
                }
            case('d'):
                bDebug = !bDebug;
                break;
            default:
                break;
                
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
