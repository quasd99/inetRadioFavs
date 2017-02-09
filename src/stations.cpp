#include "stations.h"

#include <algorithm>
#include <fstream>
#include <iostream>

/* boost */
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <json/json.h>

#include "DialogStations.h"
#include "file.h"

using namespace gats;

bool
Stations::init()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    v_s_station_keys = { "name", "address", "datetime", "homepage", "genre", 
                         "country", "city" };
    
    if ( !read_db() )
    {
        std::cerr << "  ERR:" << __PRETTY_FUNCTION__
                << ":cannot read stations-db:"
                << path_db
                << std::endl;
    }
    
    return true;
}

void Stations::add_new_station(s_station& s)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    current_stations_db_id++;
    s.id = current_stations_db_id;
    
    std::cout << "  current_stations_db_id:" << current_stations_db_id << std::endl;
    std::cout << "  s.id:" << s.id << std::endl;
    
    auto success = m_stations_db.emplace(std::make_pair( s.id, s));
    if ( !success.second )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":cannot add new station id(" << s.id << ":"
                << s.name
                << std::endl;
        return;
    }
    
    if ( !changed )
    {
        changed = true;
    }
}

void
Stations::add_station_to_recentlist(const unsigned int& id)
{
    auto pos = std::find(std::begin(v_recent_stations), 
                         std::end(v_recent_stations),
                         id
    );
    
    if (pos != std::end(v_recent_stations))
    {
        v_recent_stations.erase(pos);
    }
    
    v_recent_stations.emplace(std::begin(v_recent_stations), id);
    
    if (v_recent_stations.size() > 10)
    {
        v_recent_stations.pop_back();
    }
    
    std::cout << "Info:" << __PRETTY_FUNCTION__
              << ":id:" << id
              << std::endl;
}

s_station
Stations::get_station_by_id(const unsigned int& id)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << ":id:" << id <<  std::endl;
    // TODO validation
    return m_stations_db.at(id);
}

unsigned int
Stations::get_recentlist_first()
{
    if (!v_recent_stations.empty())
    {
        return v_recent_stations.at(0);
    }
    
    return 0;
}

std::string
Stations::get_value_by_key(const s_station& s, const std::string& key)
{
    if ( key == "name")
    {
        return s.name;
    }
    else if ( key == "address" )
    {
        return s.address;
    }
    else if ( key == "datetime" )
    {
        return s.datetime;
    }
    else if ( key == "homepage" )
    {
        return s.homepage;
    }
    else if ( key == "genre" )
    {
        return s.genre;
    }
    else if ( key == "country" )
    {
        return s.country;
    }
    else if ( key == "city" )
    {
        return s.city;
    }
    
    return "";
}

void
Stations::set_s_station_value(s_station& s,
                           const std::string& key, 
                           const std::string& value)
{
    if ( key == "name")
    {
        s.name = value;
    }
    else if ( key == "address" )
    {
         s.address = value;
    }
    else if ( key == "datetime" )
    {
         s.datetime = value;
    }
    else if ( key == "homepage" )
    {
         s.homepage = value;
    }
    else if ( key == "genre" )
    {
         s.genre = value;
    }
    else if ( key == "country" )
    {
         s.country = value;
    }
    else if ( key == "city" )
    {
         s.city = value;
    }    
}

bool Stations::read_db()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    if ( !gats::File::is_regular_file(path_db) )
    {
        return false;
    }
    
    // TODO backup
    
    std::ifstream f_db;
    f_db.open(path_db);
    if ( !f_db )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot open:" 
                  << path_db
                  << std::endl;        
        return false;
    }
    
    // read json
    Json::Reader reader;
    Json::Value root;
    
    bool parsingSuccessful = reader.parse(f_db, root);
    if ( !parsingSuccessful )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":cannot parse json:" << path_db << ":"
                << reader.getFormattedErrorMessages()
                << std::endl;
        return false;
    }

    unsigned int highest_station_id = 0;
    for ( const auto &station_id : root.getMemberNames() )
    {
        s_station s;
        
        // id = track identifier
        s.id = std::stoi(station_id);

        // read all members from track id
        for ( const auto &member : root[station_id].getMemberNames() )
        {
            // write the member value to temporal struct s_track t
            set_s_station_value( s, member, root[station_id][member].asString() );
        }
        
        // INFO
        std::cout << "  " << s.id << ":" << s.name << ":" << s.address << std::endl;
        
        // move temp struct s_track t to map m_tracks:db
        m_stations_db.emplace(std::make_pair(s.id, std::move(s)));

        // determine highest track id
        if (s.id >  highest_station_id)
        {
             highest_station_id = s.id;
        }        
    }
    current_stations_db_id =  highest_station_id;
    
    return true;
}

bool
Stations::run_stations_dialog(s_station &s)
{
    DialogStations dlg;
    dlg.set_title("Select Station");
    dlg.add_stations(m_stations_db);
    
    unsigned int station_id;
    int status = dlg.exec(station_id);
    switch (status)
    {
        case 0:
            std::cout << "Info:" << __PRETTY_FUNCTION__
                      << ":Gui User: Abortion" << std::endl;
            return false;
            break;
        case 1:            
            s = get_station_by_id(station_id);
            
            std::cout << "Info: " << __PRETTY_FUNCTION__ 
                      << "Station selected: " << s.name << std::endl;
            break;
        default:
            std::cout << "Info:" << __PRETTY_FUNCTION__
                      << ":Gui User: Abortion" << std::endl;
            return false;
            break;
    }
    
    return true;
}

bool
Stations::write_db()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    if ( !changed )
    {
        std::cout << "  no changes made..." << std::endl;
        return true;
    }
    
    if ( !gats::File::is_regular_file(path_db) )
    {
        return false;
    }
    
    // TODO backup
    // TODO check if changes are made
    
    std::ofstream f_db;
    f_db.open(path_db, std::ios::trunc);
    if ( !f_db )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot open:" 
                  << path_db
                  << std::endl;        
        return false;
    }
    
    // read json
    Json::Value root;
    std::string value;
    for (const auto &stations_id : m_stations_db)
    { // for each track-id

        // create the track-id
        std::string id = std::to_string(stations_id.first);
        // id is also a member
        root[id]["id"] = id;
        
        for (const auto &key : v_s_station_keys )
        { // for each std::string key of struct s_tracks            
            value = get_value_by_key(stations_id.second, key);
            if ( !value.empty() )
            { 
                root[id][key] = value;
            }
        }
    }
    
    // write json to file
    Json::StyledWriter writer;
    std::ofstream f_out;
    f_out.open( path_db, std::ios::out | std::ios::trunc );
    f_out << writer.write(root);
    f_out.close();
       
    return true;
}