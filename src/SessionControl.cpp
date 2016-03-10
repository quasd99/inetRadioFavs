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
    pid = (intmax_t) getpid();
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
        ss << user_homedir << "/" << app_initkeyfile;
        user_init_keyfile = ss.str();
    }

    std::cout << "pid:" << pid << std::endl;
    std::cout << "app_vlc_pidfile:" << app_vlc_pidfile << std::endl;
    std::cout << "user_init_keyfile:" << user_init_keyfile << std::endl;
}

SessionControl::SessionControl(const SessionControl& orig) {
}

SessionControl::~SessionControl() {
}

void
SessionControl::t_vlc_trackwatch()
{
    std::string track;
    get_vlc_metadata(track);
    
    std::cout << "starting thread with:" << track << std::endl;
    
    // TODO: while (unique thread-id)
    while (true)
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
            std::cout << "new track:" << track << std::endl;
        }
        sleep(3);
    }
}

int
SessionControl::init_with_keyfile()
{
// open keyfile
    Glib::KeyFile keyfile;
    try
    {
        keyfile.load_from_file(user_init_keyfile);
    }
    catch (Glib::FileError)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << "Glib::FileError:" 
                  << user_init_keyfile << std::endl;
        return 1;
    }
    catch (Glib::KeyFileError)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << "Glib::KeyFileError:"
                  << user_init_keyfile << std::endl;
        return 2;
    }
    
// read startgroup
    auto startgroup = keyfile.get_start_group();
    if (!read_keyfile_group(keyfile, startgroup, m_appsettings))
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":read_startgroup"
                  << std::endl;
        return 3;                
    }
    
    if (!m_appsettings["language"].empty())
        app_lang = m_appsettings["language"];
    else
        app_lang = "en";
    
// read all groups
    auto groups = keyfile.get_groups();
    for (const auto &group : groups)
    {
        if (group == "appsettings")
        {
            continue;
        }
        else if (group == "ui_description")
        {
            if (!read_keyfile_group_locale(keyfile, group,m_appsettings, app_lang))
            {
                std::cerr << "ERR:" << __PRETTY_FUNCTION__
                          << ":read_keyfile_group:"
                          << group << std::endl;
                return 4;
            }
        }
        else if (!read_keyfile_group (keyfile, group, m_appsettings))
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                      << ":read_keyfile_group:"
                      << group << std::endl;
            return 6;                
        }
    }
        
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
            v_recentstations.emplace_back(key);
        }
        else
        {
            map.emplace(key, keyfile.get_string(group, key));
            std::cout << group << ":" << key << ":" 
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
    if (!read_db_station(m_stations))
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__ << std::endl;
        return false;
    }
    
    // TODO: read tracks
    current_track_id = 0;
    
    return true;
}


int
SessionControl::init_player_interface()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << ":"
              << app_player_interface
              << std::endl;
    
    if (get_player_interface() == "vlc_dbus")
    {
    // command-string: start vlc without gui and as daemon
        std::string cmd("killall vlc ; cvlc -d --pidfile ");
        cmd += app_vlc_pidfile;
    // exec
        int status = std::system(cmd.c_str());
        if (status != 0)
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                      << ":cannot start:" 
                      << cmd 
                      << std::endl;
            return 1;
        }
        
        vlc_dbus = new VlcDBusInterface();
        if (vlc_dbus->init() == 0)
        {
            app_player_interface = "vlc_dbus";
        }
        else
        {
            app_player_interface = std::string();
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                      << ":cannot init:vlc_dbus" 
                      << std::endl;
            return 2;
        }
    }
    return 0;
}

std::string
SessionControl::get_player_interface()
{
    std::string ret = m_appsettings["player_interface"];
    if (ret.empty())
        ret = "vlc_dbus"; 
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

int
SessionControl::load_station (const std::string &station_address,
                              const std::string &station_name)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << ":"
              << app_player_interface << ":"
              << station_address
              << std::endl;

    if (app_player_interface == "vlc_dbus")
    {
        vlc_dbus->open_uri(station_address);
                
        std::thread track_watch(&SessionControl::t_vlc_trackwatch, this);
        track_watch.detach();
        vlc_twatch_id = track_watch.get_id();
        // TODO: unique thread-id
        std::cout << "thread_id:" << vlc_twatch_id << std::endl;
    }
    vlc_dbus->p_proxy->Volume(0.8);
    current_station = station_name;
    
    return 0;
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
    //std::cout << "status:" << status << std::endl;
// read response
    std::ifstream f_resp(respfile);
    if (f_resp)
    {   
    // read response file
        std::stringstream ss; ss << f_resp.rdbuf();
        std::string str_resp(ss.str());
    // searchstring to extract trackinfo
        std::string str_find1("'vlc:nowplaying': <'");
        std::string str_find2("'>, 'vlc:pub");
        std::size_t index1 = str_resp.find(str_find1, 0);
        if (index1 == std::string::npos)
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                      << ":cannot find:vlc:nowplaying" 
                      << std::endl;                    
            return;
        }
        index1 += str_find1.length();
        std::size_t index2 = str_resp.find(str_find2, 0);
        if (index2 == std::string::npos)
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                      << ":cannot find:vlc:publisher" 
                      << std::endl;                    
            return;
        }
    // extract trackinfo
        std::string trackinfo(str_resp.substr(index1, index2 - index1));
        //std::cout << "trackinfo:" << trackinfo << std::endl;
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
    if (app_player_interface == "vlc_dbus")
    {
        vlc_dbus->p_proxy->Play();
    }
}
void
SessionControl::player_pause()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (app_player_interface == "vlc_dbus")
    {
        vlc_dbus->p_proxy->Pause();
    }
}
void
SessionControl::player_stop()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (app_player_interface == "vlc_dbus")
    {
        vlc_dbus->p_proxy->Stop();
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
    return Glib::DateTime::create_now_local().format("%F");
}

bool
SessionControl::read_db_station(std::map<unsigned int,s_station> &stations)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    
    std::stringstream ss_db;
    ss_db << user_homedir << "/" << app_dir << "/" << app_db_station;
    std::ifstream f_db;
    f_db.open(ss_db.str());
    
    if (!f_db)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot open:" 
                  << ss_db.str()
                  << std::endl;        
        return false;
    }
    
    for (std::string buf; std::getline(f_db, buf); )
    {
        if (buf.empty())
        { // skip empty lines
            continue;
        }
        std::vector<std::string> v_tokens;
        boost::split(v_tokens, buf, boost::is_any_of("\t"),
                     boost::token_compress_on);
       
        if (v_tokens.size() == 8)
        {
            s_station s;
            s.id       = std::stoi(v_tokens[0]);
            s.name     = v_tokens[1];
            s.address  = v_tokens[2];
            s.datetime = v_tokens[3];
            s.genre    = v_tokens[4];
            s.country  = v_tokens[5];
            s.city     = v_tokens[6];
            
            // read tags
            boost::split(s.tags, v_tokens[7], boost::is_any_of("\t"),
                         boost::token_compress_on);
            
            m_stations.emplace(s.id, s);
            std::cout << "added:" << s.name << std::endl;
        }
        else
        {
            std::cerr << "ERR:" << __PRETTY_FUNCTION__
                    << ":corrupt line, station_db_tokens:"
                    << v_tokens.size()
                    << std::endl;
        }
    }
    
    return true;
}

bool
SessionControl::read_db_tracks(Json::Value &root)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    
    // file-handling
    std::stringstream ss_db;
    ss_db << user_homedir << "/" << app_dir << "/" << app_db_tracks;
    std::ifstream f_db;
    f_db.open(ss_db.str());
    
    if (!f_db)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot open:" 
                  << ss_db.str()
                  << std::endl;        
        return false;
    }
    
    // read json
    Json::Reader reader;
    
    bool parsingSuccessful = reader.parse( f_db, root );
    if ( !parsingSuccessful )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":cannot parse json:" << ss_db << ":"
                << reader.getFormattedErrorMessages()
                << std::endl;
        return false;
    }
    
    if ( root.empty() )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":empty json:"
                << std::endl;
        return false;
    }
    
    return true;
}


std::map<unsigned int,s_station>
SessionControl::get_stations()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return m_stations;
}

s_station
SessionControl::get_station_by_id(const unsigned int &id)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return m_stations.at(id);
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
SessionControl::get_current_station()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return current_station;
}

void SessionControl::get_volume()
{
}

bool
SessionControl::write_db_tracks()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    
    if (m_tracklist.empty())
    {
        // no new tracks
        return true;
    }
    else
    {
        std::cout << "  new tracks added:" << m_tracklist.size() << std::endl;
    }
    
    std::stringstream ss_tracks;
    ss_tracks << user_homedir << "/" << app_dir << "/" << app_db_tracks;

    std::ofstream f_out;
    f_out.open(ss_tracks.str().c_str(), std::ios::app);
    
    if (!f_out)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":cannot open to write:"
                << ss_tracks.str()
                << std::endl;
        return false;
    }
    
    // create JSON
    Json::Value root;
    
    for (const auto &it : m_tracklist)
    {
        // standard-infos
        std::string id = std::to_string(it.first);
        root[ id ]["id"] = id;
        root[ id ]["artist"] = it.second.artist;
        root[ id ]["title"] = it.second.title;
        root[ id ]["datetime"] = it.second.datetime;
        root[ id ]["station"] = it.second.station;
        
        // extended-infos
        if (!it.second.year.empty())
        {
            root[ id ]["year"] = it.second.year;
        }
        
        if (!it.second.url_discogs.empty())
        {
            root[ id ]["url_discogs"] = it.second.url_discogs;
        }
        
        if (!it.second.url_youtube.empty())
        {
            root[ id ]["url_youtube"] = it.second.url_youtube;
        }
    }
    
    f_out << root;
    f_out.close();
    
    return true;
}
