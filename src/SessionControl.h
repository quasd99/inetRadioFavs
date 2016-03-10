/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SessionControl.h
 * Author: fra
 *
 * Created on February 20, 2016, 2:22 PM
 */

#ifndef SESSIONCONTROL_H
#define SESSIONCONTROL_H

#include <map>
#include <thread>
#include <glibmm-2.4/glibmm.h>
#include <json/json.h>

#include "VlcDBusInterface.h"

struct s_station
{
    unsigned int id;
    std::string name;
    std::string address;
    std::string genre;
    std::string country;
    std::string city;
    std::string datetime;
    std::vector<std::string> tags;
};

struct s_track
{
    unsigned int id;
    std::string artist;
    std::string title;
    std::string station;
    std::string year;
    std::string datetime;
    std::string url_discogs;
    std::string url_youtube;
};

class SessionControl {
    int pid;
    std::string user_homedir;
    std::string user_tmpdir;
    std::string user_init_keyfile;
    
    const std::string app_dir{".gats/inetradiofavs"};
    const std::string app_initkeyfile{".gats/inetradiofavs/init.keyfile"};
    const std::string app_db_station{"stations.db"};
    const std::string app_db_tracks{"tracks.db"};
    
    std::string app_player_interface;
    std::string app_vlc_pidfile;
    std::string app_lang;
    
    std::thread::id vlc_twatch_id;
    
    std::map<Glib::ustring,Glib::ustring> m_appsettings{};
    std::map<unsigned int, s_station> m_stations{};
    std::vector<std::string> v_recentstations{};
    
    std::map<unsigned int, s_track> m_tracklist{};
    
    unsigned int current_track_id{0};
    std::string current_station;
    std::string current_track_artist;
    std::string current_track_title;
public:
    SessionControl();
    SessionControl(const SessionControl& orig);
    virtual ~SessionControl();
    
    int init_with_keyfile ();
    bool read_keyfile_group (const Glib::KeyFile &keyfile,
                             const Glib::ustring &group,
                             std::map<Glib::ustring,Glib::ustring> &map
    );
    bool read_keyfile_group_locale (const Glib::KeyFile &keyfile,
                             const Glib::ustring &group,
                             std::map<Glib::ustring,Glib::ustring> &map,
                             const std::string &locale
    );
    bool init_with_db_files();
    bool read_db_station(std::map<unsigned int,s_station> &stations);
    bool read_db_tracks(Json::Value &root);
    
    void append_new_track(const unsigned int &id, const s_track &t)  { m_tracklist.emplace( id, t ); }
    /**
     * @brief Writes the vector v_tracklist to db-file
     */
    bool write_db_tracks();
    
    /**
     * @brief
     * Init (Audio)Player-Interface
     */
    int init_player_interface();
    
    /**
     * @brief
     * Get the session's Player-Interface
     * @return std::string player interface
     */
    std::string get_player_interface();
    
    /**
     * @brief
     * If Player-Interface = vlc_dbus, kill the process by app_vlc_pidfile
     */
    bool kill_vlc_pid ();
    /**
     * @brief
     * Load a new radio-station with the Player-Interface
     * 
     * @param std::string radio-station url
     * @return int status
     */
    int load_station (const std::string &station_address,
                      const std::string &station_name);
    /**
     * @brief Invoke play for active player-interface
     */
    void player_play();
    /**
     * @brief Invoke pause for active player-interface
     */
    void player_pause();
    /**
     * @brief Invoke stop for active player-interface
     */
    void player_stop();
    /**
     * @brief Set volume for active player-interface
     * @param double volume
     */
    void set_volume(const double &vol);
    
    
    /**
     * @brief Get actual date & time as string
     * @return std::string date (Year-Month-Day)
     */
    std::string get_datetime();
    /**
     * @brief Get metadata by system call (mdbus2)
     * @param std::string&
     */
    void get_vlc_metadata(std::string &ret);
    // TODO
    void get_volume();
    void get_recent_station(std::string &station);
    void get_recent_stations(std::map<Glib::ustring,Glib::ustring> &stations);
    std::string get_current_artist();
    std::string get_current_title();
    std::string get_current_station();
    unsigned int get_current_track_id() { return ++current_track_id; };
    
    std::map<unsigned int,s_station> get_stations();
    s_station get_station_by_id(const unsigned int &id);
    
    /* threads */
    void t_vlc_trackwatch();

    /* signals */
    sigc::signal<void, std::string> signal_new_track;
private:
    VlcDBusInterface *vlc_dbus;
protected:
};

#endif /* SESSIONCONTROL_H */

