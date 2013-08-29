//
// VoiceSpearker.cpp
// NaoCar
//

#include "VoiceSpeaker.hpp"

VoiceSpeaker::VoiceSpeaker(boost::shared_ptr<AL::ALBroker>& broker) : 
    _broker(broker), _t2p(NULL),
    _mutex(), _thread(NULL), _stop(false) {
    _thread = new std::thread(&VoiceSpeaker::loop, this);
}

VoiceSpeaker::~VoiceSpeaker() {
    if (_thread) {
        _stop = true;
        _thread->join();
    }
    if (_t2p) {
        delete _t2p;
    }
}

void VoiceSpeaker::loop() {
    while (!_stop) {
        _mutex.lock();
        if (_messages.size() > 0) {
            if (!_t2p) {
                _t2p = new AL::ALTextToSpeechProxy(_broker);
            }
            std::pair<std::string, std::string> message = _messages.front();
            _messages.pop_front();
            _mutex.unlock();
            _t2p->setLanguage(message.second);
            _t2p->say(message.first);
        }
        else {
            _mutex.unlock();
            usleep(50000);
        }
    }
}

void VoiceSpeaker::say(const std::string& message, const std::string& language) {
    _mutex.lock();
    _messages.push_back(std::pair<std::string, std::string>(message, language));
    _mutex.unlock();
}
