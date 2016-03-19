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

#include "VlcDBusInterface.h"
#include "stations.h"
#include "tracks.h"

class SessionControl {
    int pid;
    std::string user_homedir;
    std::string user_tmpdir;
    //std::string user_init_keyfile;
    
    /* user paths */
    Glib::ustring app_dir{".gats/inetradiofavs"};
    Glib::ustring app_user_db_dir{".gats/inetradiofavs"};
    Glib::ustring app_initkeyfile{"init.keyfile"};
    Glib::ustring app_db_stations;
    Glib::ustring app_db_stations_selections;
    Glib::ustring app_db_tracks;
    Glib::ustring app_db_tracks_selections;
    
    /* program settings */
    std::map<Glib::ustring,Glib::ustring> m_programsettings{}; 
    
    /* last session settings*/
    std::vector<unsigned int> v_recentstations{};
    
    /* player interface */
    std::string app_player_interface;
    std::string app_vlc_pidfile;
    std::string app_lang;
       
    /* current settings */
    gats::s_station current_station;
    unsigned int    current_station_id;
    std::string     current_track_artist;
    std::string     current_track_title;
    unsigned int    current_thread_id;
    std::string     current_thread_id_str;
    std::string     current_stations_selection;
    std::string     current_tracks_selection;

    
    //std::thread::id vlc_twatch_id;
    
public:
    SessionControl();
    SessionControl(const SessionControl& orig);
    virtual ~SessionControl();
    
    gats::Stations stations;
    gats::Tracks tracks;
    
    /**
     * @brief Init the session with the settings defined in 'init.keyfile'
     * @return int 0 = success
     */
    int init_with_keyfile();
    
    /**
     * @brief Called by init_with_keyfile(): reads a specific keyfile group
     * @return bool success
     */
    bool read_keyfile_group (const Glib::KeyFile &keyfile,
                             const Glib::ustring &group,
                             std::map<Glib::ustring,Glib::ustring> &map
    );
    
    /**
     * @brief Called by init_with_keyfile(): reads a specific keyfile group
     * with language-specific definitions
     * @return bool success
     */
    bool read_keyfile_group_locale (const Glib::KeyFile &keyfile,
                             const Glib::ustring &group,
                             std::map<Glib::ustring,Glib::ustring> &map,
                             const std::string &locale
    );
    
    /**
     * @brief Reading the ad hoc database-files: stations.db and track.json
     * @return bool success
     */
    bool init_with_db_files();
    
    /**
     * @brief Initialize a gats::s_track with the current radio track data
     * @param gats::s_track& t
     */
    void init_with_current_track(gats::s_track& t);
    
    
    /**
     * @brief
     * Init (Audio)Player-Interface
     */
    int init_player_interface();
    
    /**
     * @brief Create all external paths on user-settings
     * @return bool success
     */
    bool create_program_paths(std::map<Glib::ustring,Glib::ustring> &m);
    
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
     * @param std::string radio-station address
     * @return int status
     */
    
    bool load_station(const std::string &station, const unsigned int &id);
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
    std::string get_current_station_name();
    
    void play_station();
    
    /**
     * @brief Exec xdg-open via system call
     * @param std::string url
     */
    void open_url_xdg(const std::string &url);
    
    /**
     * @brief Create a new thead-id and change current_thread_id
     * @return std::string thread_id (pid_id)
     */
    std::string update_thread_id();
        
    /**
     * @brief Called by MainWindow::on_btn_load_station() after a new station
     * is selected by DialogStations
     * @param struct gats::s_station
     */
    void set_current_station(const gats::s_station &s);
    
    /**
     * @brief Set volume for active player-interface
     * @param double volume
     */
    void set_volume(const double &vol);

    
    /* threads */
    //void t_vlc_trackwatch();
    void t_vlc_trackwatch(const std::string &t_id);

    /* signals */
    sigc::signal<void, std::string> signal_new_track;
private:
    VlcDBusInterface *vlc_dbus;
protected:
};

#endif /* SESSIONCONTROL_H */

