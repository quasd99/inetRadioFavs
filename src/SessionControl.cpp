/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SessionControl.cpp
 * Author: fra
 * 
 * Created on February 20, 2016, 2:22 PM
 */

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>

/* boost */
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

/* C */
#include <sys/types.h>
#include <unistd.h>


#include "SessionControl.h"

SessionControl::SessionControl()
{
    Glib::init();
    
   // pid
    pid = (intmax_t)getpid();
   // tmp_dir, vlc_pid
    user_tmpdir = Glib::get_tmp_dir();
    {
        std::stringstream ss;
        ss << user_tmpdir << "/irf" << pid << ".vlc.pid";
        app_vlc_pidfile = ss.str();
    }
   // home_dir, keyfile
    user_homedir = Glib::get_home_dir();
    {
        std::stringstream ss;
        ss << user_homedir << "/" << app_dir << "/" << app_initkeyfile;
        app_initkeyfile = ss.str();
    }

    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl
              << "  pid:" << pid << std::endl
              << "  app_vlc_pidfile:" << app_vlc_pidfile << std::endl
              << "  app_initkeyfile:" << app_initkeyfile << std::endl;
}

SessionControl::SessionControl(const SessionControl& orig) {
}

SessionControl::~SessionControl() {
}


void
SessionControl::t_vlc_trackwatch(const std::string& t_id)
{
    std::string track;
    get_vlc_metadata(track);
    
    std::cout << "Info:" << __PRETTY_FUNCTION__
            << ":starting thread:" << t_id << ":"
            << track
            << std::endl;

    // TODO: while (unique thread-id)
    while (t_id == current_thread_id_str)
    {
        std::string now;
        get_vlc_metadata(now);
        if (track != now && !now.empty())
        {
            track = now;
            
            std::size_t index = track.find(" - ");
            if (index != std::string::npos && (index + 3) < track.length())
            {
                current_track_artist = track.substr(0, index);
                current_track_title = track.substr(index + 3, track.length());
            }
            else
            {
                std::cerr << "ERR:" << __PRETTY_FUNCTION__
                        << ":separator ' _ ' not found:"
                        << track
                        << std::endl;
            }
            
            signal_new_track.emit(track);
            std::cout << "Info:" << __PRETTY_FUNCTION__
                    << ":new track:"
                    << track
                    << std::endl;
        }
        sleep(3);
    }
    
    std::cout << "Info:" << __PRETTY_FUNCTION__
            << ":kill thread:"
            << t_id
            << std::endl;
}


int
SessionControl::init_with_keyfile()
{
// open keyfile
    Glib::KeyFile keyfile;
    try
    {
        keyfile.load_from_file(app_initkeyfile);
    }
    catch (Glib::FileError)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << "Glib::FileError:" 
                  << app_initkeyfile << std::endl;
        return 1;
    }
    catch (Glib::KeyFileError)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << "Glib::KeyFileError:"
                  << app_initkeyfile << std::endl;
        return 2;
    }
    
   // read startgroup
    auto startgroup = keyfile.get_start_group();
    if (!read_keyfile_group(keyfile, startgroup, m_programsettings))
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":read_startgroup"
                  << std::endl;
        return 3;                
    }
    
    std::string value = m_programsettings["language"];
    if ( !value.empty() )
    {
        app_lang = value;
    }
    else
    {
        app_lang = "en";
    }
    
    value = m_programsettings["player_interface"];
    if ( !value.empty() )
    {
        app_player_interface = value;
    }
    else
    {
        app_player_interface = "cvlc_dbus";
    }
        
   // read all groups
    auto groups = keyfile.get_groups();
    for (const auto &group : groups)
    {
        if (group == "appsettings")
        {
            continue;
        }
        
        if (group == "ui_description")
        {
            if (!read_keyfile_group_locale(keyfile, group,m_programsettings, app_lang))
            {
                std::cerr << "ERR:" << __PRETTY_FUNCTION__
                          << ":read_keyfile_group:"
                          << group << std::endl;
                return 4;
            }
            continue;
        }
        
        if (!read_keyfile_group (keyfile, group, m_programsettings))
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                      << ":read_keyfile_group:"
                      << group << std::endl;
            return 6;                
        }
    }
    
    /* create paths: all (user-defined) db-filepaths */
    create_program_paths(m_programsettings);
    
    /* make the last session selections to current selection */
    current_tracks_selection = m_programsettings["track_selection"];
    current_stations_selection = m_programsettings["station_selection"];
            
    return 0;
}

bool
SessionControl::read_keyfile_group (const Glib::KeyFile &keyfile,
                    const Glib::ustring &group,
                    std::map<Glib::ustring,Glib::ustring> &map)
{
    auto keys = keyfile.get_keys(group);
    if (keys.empty())
        return false;
    
    for (const auto &key : keys)
    {
        if (group == "recent_stations")
        {
            stations.add_station_to_recentlist(keyfile.get_uint64(group, key));
        }
        else
        {
            map.emplace(key, keyfile.get_string(group, key));
            std::cout << "Info:" << __PRETTY_FUNCTION__ << ":"
                      << group << ":" << key << ":" 
                      << keyfile.get_string(group, key) << std::endl;
        }
    }

    return true;
}

bool
SessionControl::read_keyfile_group_locale (const Glib::KeyFile &keyfile,
                    const Glib::ustring &group,
                    std::map<Glib::ustring,Glib::ustring> &map,
                    const std::string &locale)
{
    auto keys = keyfile.get_keys(group);
    if (keys.empty())
        return false;
    
    for (const auto &key : keys)
    {
        map.emplace(key, keyfile.get_locale_string(group, key));
    }

    return true;
}

bool
SessionControl::init_with_db_files()
{
   /* stations */
    std::cout << "Info:" << __PRETTY_FUNCTION__ 
              << "app_db_station:"
              << app_db_stations
              << std::endl;
    
    stations.set_path_db(app_db_stations);
    stations.set_path_db_selections(app_db_stations_selections);
    stations.init(); // = read app_db_station
    
    unsigned int first_station_id = stations.get_recentlist_first();
    set_current_station(stations.get_station_by_id(first_station_id));
    
    std::cout << "Info:" << __PRETTY_FUNCTION__ << ":"
              << "current_station" << current_station.name << std::endl;
    
   /* tracks */
    std::cout << "Info:" << __PRETTY_FUNCTION__ 
              << "app_db_tracks:"
              << app_db_tracks
              << std::endl;
    
    tracks.set_path_db(app_db_tracks);
    
    // read app_db_tracks and set the current selection to current_tracks_selection
    tracks.init(current_tracks_selection); 

    return true;
}


int
SessionControl::init_player_interface()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << ":"
              << app_player_interface
              << std::endl;
    
    if (get_player_interface() == "cvlc_dbus")
    {
    // command-string: start vlc without gui and as daemon
        std::stringstream ss_cmd;
        ss_cmd << "ps aux | grep -v grep | grep vlc > /dev/null ; "
               << "if [ $? -eq 0 ] ; then killall vlc ; fi ; "
               << "cvlc -d --pidfile "
               << app_vlc_pidfile;
                
        std::string cmd{ss_cmd.str()};
    // exec
        int status = std::system(cmd.c_str());
        if (status != 0)
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                      << ":cannot exec:" 
                      << cmd 
                      << std::endl;
            return 1;
        }
        
        vlc_dbus = new VlcDBusInterface();
        if (vlc_dbus->init() == 0)
        {
            app_player_interface = "cvlc_dbus";
        }
        else
        {
            app_player_interface = std::string();
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                      << ":cannot init:cvlc_dbus" 
                      << std::endl;
            return 2;
        }
    }
    return 0;
}

bool
SessionControl::create_program_paths( std::map<Glib::ustring,
                                                     Glib::ustring> &m) 
{
    app_user_db_dir = m["app_user_db_dir"];
    // TODO dir validation
        
    app_db_stations = m["db_stations"];
    if (app_db_stations.empty())
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":m_appsettings[\"db_stations\"].empty()"
                << std::endl;
    }
   
    app_db_tracks = m["db_tracks"];
    if (app_db_tracks.empty())
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << "m_appsettings[\"db_tracks\"].empty()"
                << std::endl;
    }
    
    app_db_stations_selections = m["db_stations_selections"];
    if (app_db_stations.empty())
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":m_appsettings[\"db_stations_selections\"].empty()"
                << std::endl;
    }
   
    app_db_tracks_selections = m["db_tracks_selections"];
    if (app_db_tracks.empty())
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << "m_appsettings[\"db_tracks_selections\"].empty()"
                << std::endl;
    }
    
    return true;
}



std::string
SessionControl::get_player_interface()
{
    std::string ret = m_programsettings["player_interface"];
    if (ret.empty())
        ret = "cvlc_dbus"; 
    return ret;
}

bool
SessionControl::kill_vlc_pid()
{
    std::ifstream f_pid(app_vlc_pidfile);
    if (!f_pid)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot open:" 
                  << app_vlc_pidfile 
                  << std::endl;
        return false;
    }
    
    std::string line;
    getline(f_pid, line);
    
    std::stringstream ss_cmd;
    ss_cmd << "kill " << line;
    
    int status = std::system(ss_cmd.str().c_str());
    if (status == 0)
        return true;
    else
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot exec:" 
                  << ss_cmd.str()
                  << std::endl;
        return false;

    }
}

void
SessionControl::open_url_xdg(const std::string& url)
{
    std::stringstream ss_cmd;
    ss_cmd << "xdg-open \""
           << url
           << '"';
    std::string cmd{ss_cmd.str()};
    
    int status = std::system(cmd.c_str());
    if (status != 0)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":error exec:"
                << cmd
                << std::endl;
    }
}


void
SessionControl::play_station()
{       
    std::cout << "Info:" << __PRETTY_FUNCTION__
            << ":station-address:"
            << current_station.address
            << std::endl;
    
    if (app_player_interface == "cvlc_dbus")
    {
        vlc_dbus->open_uri(current_station.address);
        
       // generate new thread-id
        std::string t_id = update_thread_id();
       // start thread
        std::thread track_watch(&SessionControl::t_vlc_trackwatch, this, t_id);
        track_watch.detach();
    }
}

std::string
SessionControl::update_thread_id()
{
    current_thread_id++;
    
    std::stringstream ss_id;
    ss_id << pid << "_" << current_thread_id;
    
    current_thread_id_str = ss_id.str();
    
    std::cout << "Info:" << __PRETTY_FUNCTION__
            << ":new current_thread_id_str:"
            << current_thread_id_str
            << std::endl;
    
    return current_thread_id_str;
}



void
SessionControl::get_vlc_metadata(std::string &ret)
{
    
    /* vlc_dbus->p_proxy->Metadata() causes segfault libdbus-c++-1 failure
     * using system-call instead
     */
        
   // response-file
    std::stringstream ss_respfile;
    ss_respfile << user_tmpdir << "/irf" << pid << ".metadata";
    std::string respfile(ss_respfile.str());

   // call mdbus2 and write response to file
    std::stringstream ss_cmd;
    ss_cmd << "mdbus2 "
           << "org.mpris.MediaPlayer2.vlc "
           << "/org/mpris/MediaPlayer2 "
           << "org.freedesktop.DBus.Properties.Get "
           << "org.mpris.MediaPlayer2.Player Metadata"
           << " > " << respfile;
    std::string cmd(ss_cmd.str());
   // exec
    int status = std::system(cmd.c_str()); // mdbus2 returns 255 ...
    if (status == 0)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":error calling mdbus2:"
                << std::endl;        
    }
    
   // read response
    std::ifstream f_resp(respfile);
    if (f_resp)
    {   
       // read response file
        std::stringstream ss;
        ss << f_resp.rdbuf();
        
        std::string str_resp(ss.str());
       // searchstring to extract trackinfo
        std::string str_find1("'vlc:nowplaying': <");
        std::string str_find2(">, 'vlc:pub");
        std::size_t index1 = str_resp.find(str_find1, 0);
        if (index1 == std::string::npos)
        {
            std::cerr << "WARN:" << __PRETTY_FUNCTION__
                      << ":cannot find:vlc:nowplaying" << std::endl << "  "
                      << str_resp;                    
            return;
        }
        index1 += str_find1.length() + 1;
        
        std::size_t index2 = str_resp.find(str_find2, 0);
        if (index2 == std::string::npos)
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                      << ":cannot find:vlc:publisher" 
                      << std::endl;                    
            return;
        }
        index2--;
        
       // extract trackinfo
        std::string trackinfo(str_resp.substr(index1, index2 - index1));
        ret = trackinfo;
    }
    else
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":empty respfile after exec:" 
                  << cmd
                  << std::endl;        
        ret = "";
    }
}

void
SessionControl::player_play()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (app_player_interface == "cvlc_dbus")
    {
        play_station();
    }
}
void
SessionControl::player_pause()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (app_player_interface == "cvlc_dbus")
    {
        vlc_dbus->p_proxy->Pause();
    }
}
void
SessionControl::player_stop()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (app_player_interface == "cvlc_dbus")
    {
        vlc_dbus->p_proxy->Stop();
        
        // stop t_vlc_trackwatch by changing current_thread_id
        current_thread_id_str = std::string();
    }
}

void
SessionControl::set_volume(const double &vol)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    vlc_dbus->p_proxy->Volume(vol);
}

void
SessionControl::get_recent_station(std::string &station)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (v_recentstations.size() > 0)
    {
        station = v_recentstations.at(0);
    }
}

void
SessionControl::get_recent_stations(std::map<Glib::ustring,Glib::ustring> &stations)
{
    
}

std::string
SessionControl::get_datetime()
{
    return Glib::DateTime::create_now_local().format("%F_%T");
}

std::string
SessionControl::get_current_artist()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return current_track_artist;
}

std::string
SessionControl::get_current_title()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return current_track_title;
}

std::string
SessionControl::get_current_station_name()
{
    
    return current_station.name;
}

void SessionControl::get_volume()
{
}

void
SessionControl::set_current_station(const gats::s_station& s)
{
    current_station = s;
}

void
SessionControl::init_with_current_track(gats::s_track& t)
{
    t = gats::s_track{ tracks.get_new_track_id(),
            get_current_artist(),
            get_current_title(),
            get_current_station_name(),
            get_datetime()
    };            
    //    t.id       = get_current_track_id();
    //    t.artist   = get_current_artist();
    //    t.title    = get_current_title();
    //    t.station  = get_current_station_name();
    //    t.datetime = get_datetime();
}
