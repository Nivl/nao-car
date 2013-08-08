//
// BonjourDelegate.hpp
// NaoCar Remote Server
//

#ifndef _BONJOUR_DELEGATE_HPP_
# define _BONJOUR_DELEGATE_HPP_

# include "Bonjour.hpp"

class BonjourDelegate {
public:
    virtual ~BonjourDelegate(void) {}
    
    virtual void serviceBrowsed(bool error,
                                Bonjour::BrowsingType browsingType=Bonjour::BrowsingAdd,
                                std::string const& name="",
                                std::string const& type="",
                                std::string const& domain="")
    { (void)error; (void)browsingType; (void)name; (void)type; (void)domain; }
    
    virtual void serviceResolved(bool error,
                                 std::string const& hostname="",
                                 std::string const& ip="",
                                 unsigned short port=0)
    { (void)error; (void)hostname; (void)ip; (void)port; }
    
};

#endif
