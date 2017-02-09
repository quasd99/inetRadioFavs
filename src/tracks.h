#ifndef TRACKS_H
#define TRACKS_H

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <memory>

#include <glibmm-2.4/glibmm.h>

namespace gats
{
    struct s_track
    {
        // basic: always set
        unsigned int id;
        std::string artist;
        std::string title;
        std::string station;
        std::string added_datetime;
        
        // extended
        std::string release_date;
        std::string url_discogs_release;
        std::string url_youtube;
    };
 
    class Tracks
    {        
        Glib::ustring path_db;
        std::vector<std::string> v_keylist;
        bool changed = false;
        
        public:
        Tracks();
        bool init(const std::string& selection);
        
        /* map for all track-selections */
        std::map<unsigned int, s_track> m_tracks_db;
        /* map for all tracks of a selection */
        //std::unordered_map<unsigned int, s_track> m_tracks;
        /* current working map */
        std::string str_current_tracks_selection;
        std::map<unsigned int, s_track> m_current_tracks_selection;
        
        
        // The first track id is 1, so 0 can be used if a track cannot be found
        unsigned int current_track_id{1}; 

        /**
         * @brief This methods adds new track t to m_tracks.
         * When a new track should be added (via gui) a new track-id is requested
         * by calling get_new_track_id(). t.id is already using the new 
         * generated track-id. The t.id is used also by MainWindow::tview as
         * hidden id.
         * 
         * @param s_track&
         */
        void add_track(const s_track& t);
        
        /**
         * @brief Set the current selection by selection-name
         * @param std::string selection-name
         */
        void change_current_selection(const std::string& sel);
        
        /**
         * @brief Request a new track_id: Returns ++current_track_id
         * @return unsigend int current_track_id
         */
        unsigned int get_new_track_id() { return ++current_track_id; };
        
        /**
         * @brief Get current_track_id
         * @return unsigend int current_track_id
         */
        unsigned int get_track_id() { return current_track_id; };
        
        /**
         * @brief Get a value by key from a s_track reference
         * @param std::string key
         * @return auto (std::string or unsigned int)
         */
        std::string get_value_by_key(const s_track& t, const std::string& key);
        
        /**
         * @brief Remove the track with id from m_tracks
         * @param unsigned int& id (track-id)
         */
        void remove_track(const unsigned int &id);
        
        /**
         * @brief Set the path to the tracks db file
         * @param Glib::ustring& path
         */
        void set_path_db(Glib::ustring& path) { path_db = path; };

        /**
         * @brief Write string-value to s_track reference, key = can be all values
         * @value s_track& t reference to write to
         * @value std::string& key
         * @value std::string& value
         */
        void set_s_track_value(s_track &t, 
                               const std::string& key,
                               const std::string& value);
        /**
         * @brief Write a string-value to s_track reference, key = extended value
         * Can be used if the basic s_track values are already set
         * @value s_track& t reference to write to
         * @value std::string& key
         * @value std::string& value
         */
        void set_s_track_ext_value(s_track &t, 
                                   const std::string& key,
                                   const std::string& value);
        
        /**
         * @brief Read the tracks.json and save it to map m_tracks_db
         * @return bool success
         */
        bool read_tracks_db();
        
        /**
         * @brief Write m_tracks data to db_file
         * @return bool success
         */
        bool write_db();
        
        private:
    };

} // namespace gats
 
#endif // TRACKS_H
