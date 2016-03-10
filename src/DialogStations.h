/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DialogStations.h
 * Author: fra
 *
 * Created on March 4, 2016, 8:06 PM
 */

#ifndef DIALOGSTATIONS_H
#define DIALOGSTATIONS_H

#include <gtkmm-3.0/gtkmm.h>
#include "SessionControl.h"

class DialogStations : public Gtk::Dialog
{
    class Columns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Columns() { 
            add(col_name);
            add(col_address);
            add(col_datetime);
            add(col_genre);
            add(col_country);
            add(col_city);
            add(col_tags);
            add(col_id);
        }
        ~Columns() {}
        Gtk::TreeModelColumn<Glib::ustring> col_name;
        Gtk::TreeModelColumn<Glib::ustring> col_address;
        Gtk::TreeModelColumn<Glib::ustring> col_datetime;
        Gtk::TreeModelColumn<Glib::ustring> col_genre;
        Gtk::TreeModelColumn<Glib::ustring> col_country;
        Gtk::TreeModelColumn<Glib::ustring> col_city;
        Gtk::TreeModelColumn<Glib::ustring> col_tags;
        Gtk::TreeModelColumn<unsigned int> col_id; // hidden
    };

public:
    DialogStations();
    DialogStations(const DialogStations& orig);
    virtual ~DialogStations();
    
    void add_stations(const std::map<unsigned int,s_station> &m_stations);
    int exec(unsigned int &station_id);
private:
protected:
    Gtk::TreeView       tview;
    Columns             cols;
    Gtk::ScrolledWindow scrolled_window;
    Glib::RefPtr<Gtk::TreeStore> tree_model;
    Gtk::Box *vbox;
    
    unsigned int selected_id;
    
    void slot_row_activated(const Gtk::TreePath &path, Gtk::TreeViewColumn *column);
};

#endif /* DIALOGSTATIONS_H */

