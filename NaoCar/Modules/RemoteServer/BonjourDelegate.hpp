//
// BonjourDelegate.hpp
// NaoCar Remote Server
//

#ifndef _BONJOUR_DELEGATE_HPP_
# define _BONJOUR_DELEGATE_HPP_

class BonjourDelegate {
public:
    virtual ~BonjourDelegate(void) {}

    virtual void serviceRegistered(bool error,
                                   std::string const& name="")
    { (void)error; (void)name; }
};

#endif
