#pragma once


#include "ofMain.h"
#include "ofxJSON.h"


class JSONthread: public ofThread
{
public:
    /// 
    /// variable in an initialization list.
    void init()
    {
        //set the URL to the Kimono API: https://www.kimonolabs.com/apis/4k9ehldm
        url = "https://www.kimonolabs.com/api/4k9ehldm?apikey=No7h5YUcyCWbbn7xjG0ukUrDZTpjWFUU";
        //see if the API works
        bool parsingSuccessful = json.open(url);
        
        if(parsingSuccessful)
        {
            ofLogNotice("JSONthread::Init") << json.getRawString(true);
        }
        
      //  humidity = wind = temp = visibility = pressure = 0;

    }
    
    
    /// Our implementation of threadedFunction.
    void threadedFunction()
    {
        while(isThreadRunning())
        {
            
            ofLogNotice("JSONthread::threadedFunction") << "waiting for info...";
            bool parsingSuccessful = json.open(url);
            
            if(parsingSuccessful)
            {
                json.getRawString(true);
                ofLogNotice("JSONthread::threadedFunction") << "info received";
                            
                // Attempt to lock the mutex.  If blocking is turned on,
                if(lock())
                {
                    ofLogNotice("JSONthread::threadedFunction") << "locked";
                    //do stuff
                     humidity = json["results"]["collection1"][0]["humidity"].asString();
                     pressure = json["results"]["collection1"][0]["pressure"].asString();
                     temp = json["results"]["collection1"][0]["temp"].asString();
                     visibility = json["results"]["collection1"][0]["visibility"].asString();
                     wind = json["results"]["collection1"][0]["wind"].asString();
                    
                    ofLogNotice("JSONthread::threadedFunction") << "we got data: " << humidity << " " << pressure << " " << temp << " " << visibility;
                    
                    unlock();
                    ofLogNotice("JSONthread::threadedFunction") << "unlocked. going to sleep...";
                    // Sleep for 60 seconds.
                    sleep(1000 * 30);
                }
                else
                {
                    // If we reach this else statement, it means that we could not
                    // lock our mutex, and so we do not need to call unlock().
                    // Calling unlock without locking will lead to problems.
                    ofLogWarning("JSONthread::threadedFunction") << "Unable to lock mutex.";
                }
            }
        }
    }
    
    ofxJSONElement json;
    std::string url;
    string humidity, pressure, temp, visibility, wind;
    
};
