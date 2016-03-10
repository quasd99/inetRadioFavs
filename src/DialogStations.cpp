/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DialogStations.cpp
 * Author: fra
 * 
 * Created on March 4, 2016, 8:06 PM
 */

#include <iostream>
#include <sstream>
#include "DialogStations.h"
#include "SessionControl.h"

DialogStations::DialogStations() 
: tview(),
  cols(),
  scrolled_window(),
  tree_model(Gtk::TreeStore::create(cols))
{
    set_default_size(800, 600);
    
    tview.set_model(tree_model);
    tview.append_column("Name", cols.col_name);
    tview.append_column("Address", cols.col_address);
    tview.append_column("Genre", cols.col_genre);
    tview.append_column("Added", cols.col_datetime);
    tview.append_column("Country", cols.col_country);
    tview.append_column("City", cols.col_city);
    tview.append_column("Tags", cols.col_tags);
    tview.set_margin_bottom(10);
    tview.set_activate_on_single_click(true);
    tview.signal_row_activated().connect(sigc::mem_fun(this, 
                                         &DialogStations::slot_row_activated));
    
    scrolled_window.add(tview);
    
    add_button(Gtk::Stock::CANCEL, 0);
    add_button(Gtk::Stock::MEDIA_PLAY, 1);
    
    vbox = get_content_area();
    vbox->set_orientation(Gtk::ORIENTATION_VERTICAL);
    vbox->pack_start(scrolled_window, Gtk::PACK_EXPAND_WIDGET, 0);
    vbox->set_homogeneous(FALSE);
    
    show_all();    
}

DialogStations::DialogStations(const DialogStations& orig) {
}

DialogStations::~DialogStations() {
}

int
DialogStations::exec(unsigned int &id)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    
    int status = run();
    if (status == 1)
    {
        if (selected_id >= 0)
        {
            id = selected_id;    
        }
        else
        {
            id = 0;
        }
    }
    return status;
}

void
DialogStations::add_stations(const std::map<unsigned int,s_station> &m_stations)
{
    std::cout << "Debug:" << __FUNCTION__ << std::endl;
    for (const auto &it : m_stations)
    {
        Gtk::TreeModel::Row row = *(tree_model->append());
        row[cols.col_id] = it.first;
        row[cols.col_name] = it.second.name;
        row[cols.col_address] = it.second.address;
        row[cols.col_datetime] = it.second.datetime;
        row[cols.col_genre] = it.second.genre;
        row[cols.col_country] = it.second.country;
        row[cols.col_city] = it.second.city;
        
        // tags
        std::stringstream ss;
        for (const auto &tag : it.second.tags)
        {
            ss << tag << " ";
        }
        row[cols.col_tags] = ss.str();
    }
    tview.resize_children();
}

void
DialogStations::slot_row_activated(const Gtk::TreePath &path,
                                       Gtk::TreeViewColumn *column)
{   
    Gtk::TreeModel::iterator iter = tree_model->get_iter (path); // row
    if (iter)
    {
        Gtk::TreeModel::Row row = *iter; // column
        selected_id = row[cols.col_id];
        std::cout << __FUNCTION__ << ":" << row[cols.col_name] << std::endl;
    }
}