/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MainWindow.h
 * Author: fra
 *
 * Created on February 20, 2016, 2:22 PM
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm-3.0/gtkmm.h>
#include "SessionControl.h"
#include "UiToolLabel.h"
#include "UiToolVolumeButton.h"
#include "uiexttreeview.h"

class MainWindow : public Gtk::Window
{
    // trackview
//    class Columns : public Gtk::TreeModel::ColumnRecord
//    {
//    public:
//        Columns() {
//            add(col_id);
//            add(col_btn_discogs);
//            add(col_artist);
//            add(col_title);
//            add(col_datetime);
//            add(col_station);
//            
//        }
//        ~Columns() {}
//        Gtk::TreeModelColumn<unsigned int> col_id; // hidden
//        Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> col_btn_discogs;
//        Gtk::TreeModelColumn<Glib::ustring> col_artist;
//        Gtk::TreeModelColumn<Glib::ustring> col_title;
//        Gtk::TreeModelColumn<Glib::ustring> col_datetime;
//        Gtk::TreeModelColumn<Glib::ustring> col_station;        
//    };
    
    
    
    // Session
    SessionControl controller;
    
    // container
    Gtk::Box vbox;
    Gtk::Box hbox;
    Gtk::Toolbar mainbar;
    Gtk::Toolbar trackbar;

    // menus
    Gtk::Menu menu_station;
    Gtk::MenuItem mi_station_show_manager;
    Gtk::SeparatorMenuItem menu_sep1;
    Gtk::MenuItem mi_station_new_url;
    Gtk::MenuItem mi_station_new_pls;

    // mainbar-items
    Gtk::ToolButton btn_settings;
    Gtk::SeparatorToolItem sep1;
    Gtk::MenuToolButton btn_load_station;
    Gtk::ToolButton btn_play;
    Gtk::ToolButton btn_stop;
    gats::UiToolVolumeButton btn_vol;
    Gtk::SeparatorToolItem sep2;
    gats::UiToolLabel lbl_station;
    
    // trackbar-items
    Gtk::ToolButton btn_add_track;
    Gtk::SeparatorToolItem sep3;
    gats::UiToolLabel lbl_track;
    
    // states
    bool is_playing{false};
    bool is_paused{false};
public:
    MainWindow();
    MainWindow(const MainWindow& orig);
    virtual ~MainWindow();
    
    int init_gui();
    int init_session();
    bool init_trackview();
    void run_station_dialog();
protected:
    /* mainbar */
    void on_btn_settings();
    void on_btn_load_station();
    void on_show_station_menu();
    void on_station_show_manager();
    void on_station_new_url();
    void on_station_new_pls();
    void on_btn_play();
    void on_btn_pause();
    void on_btn_stop();
    void on_volume_changed(double vol);
    
    void load_station(const std::string &station, const bool &url);
    void set_station_lbl(const std::string &station);

    /* trackbar */
    void on_add_track();
    void on_new_track(std::string track);
    
    /* trackview */
    gats::UiExtTreeview tview;
    Gtk::ScrolledWindow scrolled_window;
};

#endif /* MAINWINDOW_H */
