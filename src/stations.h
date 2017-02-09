#ifndef STATIONS_H
#define STATIONS_H

#include <glibmm-2.4/glibmm.h>

namespace gats
{

    struct s_station
    {
        unsigned int id;
        std::string name;
        std::string address;
        std::string datetime;
        std::string homepage;
        std::string genre;
        std::string country;
        std::string city;
        std::vector<std::string> tags;
    };
    
    class Stations
    {
        std::map<unsigned int, s_station> m_stations_db;
        std::vector<unsigned int> v_recent_stations;
        std::vector<std::string> v_s_station_keys;
        unsigned int current_stations_selection{0};
        unsigned int current_stations_db_id{1};
        Glib::ustring path_db;
        Glib::ustring path_db_selections;
        bool changed = false;
        
        public:
        Stations() : Stations("") {};
        Stations(const Glib::ustring &db) : path_db(db) {};
        
        /**
         * @brief Read the db-file and init the data
         * @return bool success
         */
        bool init();
        
        /**
         * @brief If a new station is loaded add it to v_recent_stations and reorder
         * @param unsigned int station-id
         */
        void add_station_to_recentlist(const unsigned int &id);
        
        /**
         * @brief Add a new station to m_stations_db
         * @param s_station reference
         */
        void add_new_station(s_station &s);
        
    // db actions
        bool read_db();
        bool write_db();
        
    // getter
        /**
         * @brief Returns the corresponding struct s_station by id
         * @return struct s_station
         */
        s_station get_station_by_id(const unsigned int &id);
        
        /**
         * @brief Returns the value of given key from struct s_station
         * @param s_station reference
         * @param std::string key
         * @return std::string value
         */
        std::string get_value_by_key(const s_station& s, const std::string& key);
        
        /**
         * @brief Get the first entry from the recentlist
         * @return unsigend int station-id
         */
        unsigned int get_recentlist_first();
        
        
    // setter
        
        /**
         * @brief Set the path to stations.db
         * @param Glib::ustring db
         */
        void set_path_db(const Glib::ustring &db) { path_db = db; };
        
        /**
         * @brief Set the path to stations.db
         * @param Glib::ustring db
         */
        void set_path_db_selections(const Glib::ustring &db) { path_db_selections = db; };
        
        /**
         * @brief Write the value for the given key to the struct s_station reference
         * @param s_station reference to write to
         * @param std::string key
         * @param std::string value
         */
        void set_s_station_value(s_station& s,
                                 const std::string& key,
                                 const std::string& value);
        
    // misc
        bool run_stations_dialog(s_station &s);
        private:
    };

} // namespace gats
 
#endif // STATIONS_H
