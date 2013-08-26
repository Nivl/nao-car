//
// VoiceSpearker.hpp
// NaoCar
//

#ifndef _VOICE_SPEAKER_
# define _VOICE_SPEAKER_

# include <alcommon/almodule.h>
# include <boost/asio.hpp>
# include <boost/thread/thread.hpp>
# include <alproxies/altexttospeechproxy.h>
# include <thread>
# include <mutex>

class VoiceSpeaker {
public:
    VoiceSpeaker(boost::shared_ptr<AL::ALBroker>& broker);
    ~VoiceSpeaker();

    void loop();
    void say(const std::string& message, const std::string& language="French");
private:
    AL::ALTextToSpeechProxy       _t2p;
    std::list<std::pair<std::string, std::string> >	_messages;
    std::mutex			_mutex;
    std::thread*			_thread;
    bool				_stop;
};

#endif
