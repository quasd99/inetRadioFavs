/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VlcDBusInterface.h
 * Author: fra
 *
 * Created on February 22, 2016, 3:26 PM
 */

#ifndef VLCDBUSINTERFACE_H
#define VLCDBUSINTERFACE_H

#include <dbus-c++/dbus.h>
#include <dbus-c++/types.h>
#include "VlcMediaPlayer2_proxy.h"


class MediaPlayer2_proxy
: public org::mpris::MediaPlayer2_proxy,
  public DBus::IntrospectableProxy,
  public DBus::ObjectProxy
{
public:
    MediaPlayer2_proxy(DBus::Connection& connection, const char* path, const char* name)
    : DBus::ObjectProxy(connection, path, name) {
    }
};

class Player_proxy
: public org::mpris::MediaPlayer2::Player_proxy,
  public DBus::IntrospectableProxy,
  public DBus::ObjectProxy
{
public:
    Player_proxy(DBus::Connection& connection, const char* path, const char* name)
    : DBus::ObjectProxy(connection, path, name) {
    }    
};
      
class TrackList_proxy
: public org::mpris::MediaPlayer2::TrackList_proxy,
  public DBus::IntrospectableProxy,
  public DBus::ObjectProxy
{
public:
    TrackList_proxy(DBus::Connection& connection, const char* path, const char* name)
    : DBus::ObjectProxy(connection, path, name) {
    }
    
    virtual void TrackListReplaced(const std::vector< ::DBus::Path >& argin0, const ::DBus::Path& argin1) override;
    virtual void TrackAdded(const std::map< std::string, ::DBus::Variant >& argin0, const ::DBus::Path& argin1) override;
    virtual void TrackRemoved(const ::DBus::Path& argin0) override;
    virtual void TrackMetadataChanged(const ::DBus::Path& argin0, const std::map< std::string, ::DBus::Variant >& argin1) override;

};

class VlcDBusInterface {
    
public:
    VlcDBusInterface();
    friend class SessionControl;
    
    int init();
    bool open_uri (const std::string &url);
    
    void get_metadata ();
private:
    MediaPlayer2_proxy *m_proxy;
    Player_proxy *p_proxy;
    TrackList_proxy *t_proxy;
};


#endif /* VLCDBUSINTERFACE_H */

