/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MainWindow.cpp
 * Author: fra
 * 
 * Created on February 20, 2016, 2:22 PM
 */

#include <iostream>
#include <gtkmm-3.0/gtkmm/scalebutton.h>
#include "MainWindow.h"
#include "DialogStations.h"

MainWindow::MainWindow()
: Gtk::Window()
{
    int status = init_session();
    std::cout << __FUNCTION__ << ":init_session:status:" << status << std::endl;
    // TODO default-values if cannot read keyfile
    
    status = init_gui();
    std::cout << __FUNCTION__ << ":init_gui:status:" << status << std::endl;
}

MainWindow::MainWindow(const MainWindow& orig) {
}

MainWindow::~MainWindow() 
{
    std::cout << "EXIT" << std::endl;
    
    if (!controller.kill_vlc_pid())
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot close vlc by pidfile"
                  << std::endl;
    }
    
    if (!controller.write_db_tracks())
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":error writing tracks-db:"
                << std::endl;
    }
}

int
MainWindow::init_session()
{
   // init session
    int status = controller.init_with_keyfile();
    if (status != 0)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":init_with_keyfile:"
                  << status
                  << std::endl;
        // TODO init_fallback ();
        return 1;
    }
    
    if (!controller.init_with_db_files())
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":init_with_db_files:"
                  << status
                  << std::endl;
        // TODO Backup?
        return 2;
    }
    
    status = controller.init_player_interface();
    if (status != 0)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":init_player_interface"
                  << std::endl;
        // Try to fix it via gui
        return 3;
    }
    
    controller.signal_new_track.connect(sigc::mem_fun(*this, 
                                       &MainWindow::on_new_track));
    
    btn_vol.btn_vol.set_value(0.8);
    
    return 0;
}

int
MainWindow::init_gui()
{
    set_title("InetRadioFavs");
    set_default_size(800, 600);
    
    // menu
    mi_station_show_manager.set_label("Show Station Manager");
    mi_station_show_manager.signal_activate().connect(sigc::mem_fun(*this, 
                                       &MainWindow::on_station_show_manager));
    mi_station_new_url.set_label("New Station Url");
    mi_station_new_url.signal_activate().connect(sigc::mem_fun(*this, 
                                       &MainWindow::on_station_new_url));
    mi_station_new_pls.set_label("New Station Pls");
    mi_station_new_pls.signal_activate().connect(sigc::mem_fun(*this, 
                                       &MainWindow::on_station_new_pls));
    
    /* ! show for all menu-items ! */
    mi_station_show_manager.show();
    menu_sep1.show();
    mi_station_new_url.show();
    mi_station_new_pls.show();
    
    menu_station.append(mi_station_show_manager);
    menu_station.append(menu_sep1);
    menu_station.append(mi_station_new_url);
    menu_station.append(mi_station_new_pls);
    
    /* mainbar */
    btn_settings.set_stock_id(Gtk::Stock::PROPERTIES);
    btn_load_station.set_stock_id(Gtk::Stock::ADD);
    btn_load_station.set_menu(menu_station);
    btn_load_station.set_arrow_tooltip_text("Set a new station");
    btn_load_station.signal_show_menu().connect(sigc::mem_fun(*this, 
                                              &MainWindow::on_show_station_menu));
    btn_play.set_stock_id(Gtk::Stock::MEDIA_PLAY);
    btn_stop.set_stock_id(Gtk::Stock::MEDIA_STOP);

    mainbar.append(btn_settings, sigc::mem_fun(*this, &MainWindow::on_btn_settings));
    mainbar.append(sep1);
    mainbar.append(btn_load_station, sigc::mem_fun(*this, &MainWindow::on_btn_load_station));  
    mainbar.append(btn_play);
    btn_play.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_btn_play));
    mainbar.append(btn_stop, sigc::mem_fun(*this, &MainWindow::on_btn_stop));
    mainbar.append(btn_vol);
    btn_vol.btn_vol.signal_value_changed().connect(sigc::mem_fun(*this, 
                                              &MainWindow::on_volume_changed));
    mainbar.append(sep2);
    mainbar.append(lbl_station);
    lbl_station.set_text("http://16bit.fm/play/16bit.fm_192.m3u"); // TODO recent
    
    /* trackbar */
    btn_add_track.set_stock_id(Gtk::Stock::APPLY);
    trackbar.append(btn_add_track, sigc::mem_fun(*this, &MainWindow::on_add_track));
    trackbar.append(sep3);
    trackbar.append(lbl_track);
    lbl_track.set_text("...");
    
    /* trackview */
    init_trackview();
    
    /* container */
    //hbox.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    //hbox.pack_start (mainbar, Gtk::PACK_EXPAND_WIDGET, 0);

    vbox.set_orientation(Gtk::ORIENTATION_VERTICAL);
    //vbox.pack_start (hbox, Gtk::PACK_SHRINK, 0);
    vbox.pack_start (mainbar, Gtk::PACK_SHRINK, 0);
    vbox.pack_start (trackbar, Gtk::PACK_SHRINK, 0);
    vbox.pack_start (scrolled_window, Gtk::PACK_EXPAND_WIDGET, 0);

    add(vbox);
    show_all();
    return 0;
}

bool
MainWindow::init_trackview()
{
    tview.init();
    scrolled_window.add(tview);
    scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    
    Json::Value root;
    if ( !controller.read_db_tracks(root) )
    {
        return false;
    }
    
    unsigned int highest_id = 0;
    
    for (const auto &id : root.getMemberNames())
    {
        s_track t;
        t.id = stoi(id);
        if (t.id > highest_id)
        {
            highest_id = t.id;
        }
        
        for (const auto &member : root[id].getMemberNames())
        {
            if (member == "artist")
            {
                t.artist = root[id][member].asString();
            }
            else if (member == "title")
            {
                t.title = root[id][member].asString();
            }
            else if (member == "datetime")
            {
                t.datetime = root[id][member].asString();
            }
            else if (member == "station")
            {
                t.station = root[id][member].asString();
            }
            //std::cout << member << ":" << root[id][member].asString() << std::endl;
        }
        tview.add_track(t);
        
    }
    
    
    
    return true;
}

void
MainWindow::on_btn_settings()
{
    
}

void
MainWindow::on_btn_load_station()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    run_station_dialog();
}

void
MainWindow::run_station_dialog()
{
    std::vector<std::string> list;
    list.emplace_back("http://16bit.fm/play/16bit.fm_192.m3u");
    list.emplace_back("http://evans.hochschulradio.rwth-aachen.de:8000/hoeren/high.m3u");
    DialogStations dlg;
    dlg.set_title("Select Station");
    dlg.add_stations(controller.get_stations());
    unsigned int station_id;
    int status = dlg.exec(station_id);
    switch (status)
    {
        case 0:
            std::cout << "Abortion" << std::endl;
            break;
        case 1:
            on_btn_stop();
            s_station s = controller.get_station_by_id(station_id);
            controller.load_station(s.address, s.name);
            set_station_lbl(s.name);
            on_btn_play();
            
            std::cout << "Station selected: " << s.name << std::endl;
            break;
    }
}

void
MainWindow::set_station_lbl(const std::string& station)
{
    lbl_station.set_text(station);
}

void
MainWindow::on_btn_play()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    
    if (!is_playing)
    { // activate, only Stop-Button & load_station deactivate is_playing
        is_playing = true;
        is_paused = false;
        btn_play.set_stock_id(Gtk::Stock::MEDIA_PAUSE);
        controller.player_play();
        return;
    }
    
    // toggle play/pause mode
    if (!is_paused)
    {
        is_paused = true;
        btn_play.set_stock_id(Gtk::Stock::MEDIA_PLAY);
        controller.player_pause();
    }
    else
    {
        is_paused = false;
        btn_play.set_stock_id(Gtk::Stock::MEDIA_PAUSE);
        controller.player_play();
    }
}

void
MainWindow::on_btn_stop()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    btn_play.set_stock_id(Gtk::Stock::MEDIA_PLAY);
    is_playing = false;
    is_paused = false;
    controller.player_stop();
}

void
MainWindow::on_volume_changed(double vol)
{
    controller.set_volume(vol);
}

void
MainWindow::on_show_station_menu()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void
MainWindow::on_station_show_manager()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    run_station_dialog();
}

void
MainWindow::on_station_new_url()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void
MainWindow::on_station_new_pls()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void
MainWindow::on_new_track(std::string track)
{
    std::cout << __PRETTY_FUNCTION__ << ":" << track << std::endl;
    lbl_track.set_text(track);
}

void
MainWindow::on_add_track() 
{
    std::cout << __PRETTY_FUNCTION__ << ":" << std::endl;
    
    s_track t;
    t.id       = controller.get_current_track_id();
    t.artist   = controller.get_current_artist();
    t.title    = controller.get_current_title();
    t.datetime = controller.get_datetime();
    t.station  = controller.get_current_station();
    
    controller.append_new_track(t.id, t);
    tview.add_track(t); 
}

//void
//MainWindow::slot_row_activated(const Gtk::TreePath& path,
//                                    Gtk::TreeViewColumn* column)
//{
//    std::cout << __PRETTY_FUNCTION__ << ":" << std::endl;
//    Gtk::TreeModel::iterator iter = tree_model->get_iter (path); // row
//    if (iter)
//    {
//        Gtk::TreeModel::Row row = *iter; // column
//        selected_id = row[cols.col_id];
//        std::cout << __FUNCTION__ << ":" << row[cols.col_artist] << std::endl;
//    }
//}
//
//void
//MainWindow::on_btn_discogs()
//{
//    std::cout << __PRETTY_FUNCTION__ << ":" << std::endl;
//}
