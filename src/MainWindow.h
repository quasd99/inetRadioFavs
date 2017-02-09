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
#include "uidialognewstation.h"
#include "stations.h"
#include "tracks.h"

class MainWindow : public Gtk::Window
{
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
    
   // tview-control
    Gtk::Box hbox_tviewcontrol;
    Gtk::Button btn_tview_delete{Gtk::Stock::DELETE};
    
   // states
    bool is_playing{false};
    bool is_paused{false};
    
public:
    MainWindow();
    MainWindow(const MainWindow& orig);
    virtual ~MainWindow();
    
    /**
     * @brief Calling all init-methods in right order
     */
    void init();
    
    /**
     * @brief Calling:
     * SessionControl::init_with_keyfile
     * SessionControl::init_with_db_files
     * SessionControl::init_player_interface
     * @return bool success if all methods are executed without error
     */
    bool init_session();
    
    /**
     * @brief Init the track-view-gui components
     * Calling SessionControl::controller.read_db_tracks to read the tracks-
     * database file and add it to UiTextTreeView::tview
     * @return bool success
     */
    bool init_trackview();
    
    /**
     * @brief Initialize all needed gui-elements
     */
    void init_gui();
    
protected:
   /* mainbar signal-slot-methods */
   // mainbar buttons
    void on_btn_settings();
    void on_btn_load_station();
    void on_btn_play();
    void on_btn_pause();
    void on_btn_stop();
   // mainbar station menu
    void on_show_station_menu();
    void on_menustation_show();
    void on_menustation_add();
    void on_station_new_pls();
   // mainbar btn_vol signal-handle
    void on_volume_changed(double vol);
   // mainbar set-method
    void set_lbl_station(const std::string &station);
    
   /* trackbar signal-slot-methods */
    void on_btn_add_track();
    void on_event_new_track(std::string track);
   // trackbar set-method
    void set_lbl_track(const std::string &track);
    
    void on_btn_tview_delete();
    
   /* trackview elements */
    gats::UiExtTreeview tview;
    Gtk::ScrolledWindow scrolled_window;
};

#endif /* MAINWINDOW_H */
