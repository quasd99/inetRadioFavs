/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VlcDBusInterface.cpp
 * Author: fra
 * 
 * Created on February 22, 2016, 3:26 PM
 */

#include "VlcDBusInterface.h"

void
TrackList_proxy::TrackListReplaced(const std::vector<::DBus::Path>& argin0, const ::DBus::Path& argin1)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void
TrackList_proxy::TrackAdded(const std::map<std::string, ::DBus::Variant>& argin0,
                            const ::DBus::Path& argin1)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void
TrackList_proxy::TrackRemoved(const ::DBus::Path& argin0)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void
TrackList_proxy::TrackMetadataChanged(const ::DBus::Path& argin0,
                     const std::map< std::string,::DBus::Variant >& argin1)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    for (const auto &it : argin1)
    {
        auto val = it.second;
        std::cout << __PRETTY_FUNCTION__ << ":" << it.first << ":"
                  << std::endl;
    }
}

static DBus::BusDispatcher dispatcher;

VlcDBusInterface::VlcDBusInterface() {
}

int
VlcDBusInterface::init()
{
                                           
    static const char* vlc_service_name = "org.mpris.MediaPlayer2.vlc";
    static const char* vlc_object_path = "/org/mpris/MediaPlayer2";
    
    DBus::default_dispatcher = &dispatcher;
    DBus::Connection bus = DBus::Connection::SessionBus();
    
    // bus, object, service-name 
    m_proxy = new MediaPlayer2_proxy(bus, vlc_object_path, vlc_service_name);
    if (!p_proxy)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot init:vlc MediaPlayer2_proxy" 
                  << std::endl;
        return 1;
    }
    
    p_proxy = new Player_proxy(bus, vlc_object_path, vlc_service_name);
    if (!p_proxy)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot init:vlc Player_proxy" 
                  << std::endl;
        return 2;
    }
    
    t_proxy = new TrackList_proxy(bus, vlc_object_path, vlc_service_name);
    if (!p_proxy)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot init:vlc TrackList_proxy" 
                  << std::endl;
        return 4;
    }
    
    return 0;
}


bool
VlcDBusInterface::open_uri (const std::string &url)
{
    if (p_proxy)
    {
        p_proxy->OpenUri(url);
        return true;
    }
    else
    {
        std::cout << "Err:" << __PRETTY_FUNCTION__
                  << ":cannot open uri"
                  << url
                  << std::endl;
        return false;
    }
}

