#include <fstream>
#include <iostream>
#include <map>

#include <json/json.h>

#include "tracks.h"

using namespace gats;
 
Tracks::Tracks()
{
}

bool
Tracks::init(const std::string& selection)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    // define std::string keys of struct s_track
    v_keylist = { "artist",
                  "title", 
                  "station",
                  "added_datetime",
                  "release_date",
                  "url_discogs_release",
                  "url_youtube"
    };

    
    if ( !read_tracks_db() )
    {
        //TODO
    }
    
    
    // init current track-selection by param selection
//    if ( track_selection == selection )
//    {
//        // copy
//        m_current_tracks_selection = m_tracks;
//        str_current_tracks_selection = selection;
//        current_track_id = highest_track_id;
//    }
           
    return true;
}


void
Tracks::add_track(const s_track& t)
{
    m_tracks_db.emplace(std::make_pair(t.id, t));
    if ( !changed )
    {
        changed = true;
    }
}

std::string
Tracks::get_value_by_key(const s_track& t, const std::string& key)
{
    if (key == "artist")
    {
        return t.artist;
    }
    else if (key == "title")
    {
        return t.title;
    }
    else if (key == "station")
    {
        return t.station;
    }
    else if (key == "added_datetime")
    {
        return t.added_datetime;
    }
    else if (key == "release_date")
    {
        return t.release_date;
    }
    else if (key == "url_discogs_release")
    {
        return t.url_discogs_release;
    }
    else if (key == "url_youtube")
    {
        return t.url_youtube;
    }
    
    return std::string();
}

void
Tracks::change_current_selection(const std::string& sel)
{
}

bool
Tracks::read_tracks_db()
{
    // INFO
    std::cout << "Info:" << __PRETTY_FUNCTION__
            << ":read:"
            << path_db
            << std::endl;
    
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
        
    
    std::string track_selection;
    unsigned int highest_track_id = 0;
    for ( const auto &track_id : root.getMemberNames() )
    {
        s_track t;
        
        // id = track identifier
        t.id = std::stoi(track_id);

        // read all members from track id
        for ( const auto &member : root[track_id].getMemberNames() )
        {
            // write the member value to temporal struct s_track t
            set_s_track_value( t, member, root[track_id][member].asString() );
        }
        
        // INFO
        std::cout << "  " << t.id << ":" << t.artist << ":" << t.title << std::endl;
        
        // move temp struct s_track t to map m_tracks:db
        m_tracks_db.emplace(std::make_pair(t.id, std::move(t)));

        // determine highest track id
        if (t.id > highest_track_id)
        {
            highest_track_id = t.id;
        }        
    }
    
    current_track_id = highest_track_id;
    
    return true;
}

void
Tracks::remove_track(const unsigned int& id)
{
    m_tracks_db.erase(id);
    if ( !changed )
    {
        changed = true;
    }
}

void
Tracks::set_s_track_value(s_track& t, 
                          const std::string& key,
                          const std::string& value)
{
    if (key == "artist")
    {
        t.artist = value;
    }
    else if (key == "title")
    {
        t.title = value;
    }
    else if (key == "station")
    {
        t.station = value;
    }
    else if (key == "added_datetime")
    {
        t.added_datetime = value;
    }
    else if (key == "release_date")
    {
        t.release_date = value;
    }
    else if (key == "url_discogs_release")
    {
        t.url_discogs_release = value;
    }
    else if (key == "url_youtube")
    {
        t.url_youtube = value;
    }
}

void
Tracks::set_s_track_ext_value(s_track& t, 
                          const std::string& key,
                          const std::string& value)
{
    if (key == "release_date")
    {
        t.release_date = value;
    }
    else if (key == "url_discogs_release")
    {
        t.url_discogs_release = value;
    }
    else if (key == "url_youtube")
    {
        t.url_youtube = value;
    }
}

bool
Tracks::write_db()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;

    if ( !changed )
    {
        std::cout << "  no changes made..." << std::endl;
        return true;
    }
            
    s_track t;
    std::string value;
    Json::Value root;
    for (const auto &track_id : m_tracks_db)
    { // for each track-id
        
        // create the track-id
        std::string id = std::to_string(track_id.first);
        // id is also a member
        root[id]["id"] = id;
        
        for (const auto &key : v_keylist )
        { // for each std::string key of struct s_tracks            
            value = get_value_by_key(track_id.second, key);
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
