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
#include "stations.h"

MainWindow::MainWindow()
: Gtk::Window()
{
    init();
}

MainWindow::MainWindow(const MainWindow& orig) {
}

MainWindow::~MainWindow() 
{
    std::cout << "EXIT" << std::endl;
    
    if ( !controller.kill_vlc_pid() )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":cannot close vlc by pidfile"
                  << std::endl;
    }
    
    if ( !controller.tracks.write_db() )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":error writing tracks-db:"
                << std::endl;
    }
    
    if ( !controller.stations.write_db() )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":error writing stations-db:"
                << std::endl;
    }
}

void
MainWindow::init()
{
    bool status = init_session();
    if (!status)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":error init_session()"
                << std::endl;
    }
    init_trackview();
    init_gui();    
    
}

bool
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
        return false;
    }
    
    if (!controller.init_with_db_files())
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":init_with_db_files:"
                  << status
                  << std::endl;
        // TODO Backup?
        return false;
    }
    
    status = controller.init_player_interface();
    if (status != 0)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                  << ":init_player_interface"
                  << std::endl;
        // Try to fix it via gui
        return false;
    }
    
    controller.signal_new_track.connect(sigc::mem_fun(*this, 
                                       &MainWindow::on_event_new_track));
    btn_vol.btn_vol.set_value(0.8);
    
    return true;
}

void
MainWindow::init_gui()
{
    set_title("InetRadioFavs");
    set_default_size(800, 600);
    
   /* menu */
    mi_station_show_manager.set_label("Show Station Manager");
    mi_station_show_manager.signal_activate().connect(sigc::mem_fun(*this, 
                                       &MainWindow::on_menustation_show));
    mi_station_new_url.set_label("New Station Url");
    mi_station_new_url.signal_activate().connect(sigc::mem_fun(*this, 
                                       &MainWindow::on_menustation_add));
    mi_station_new_pls.set_label("New Station Pls");
    mi_station_new_pls.signal_activate().connect(sigc::mem_fun(*this, 
                                       &MainWindow::on_station_new_pls));
    
    // ! show for all menu-items, show_all() doesn't work !
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
    lbl_station.set_text(controller.get_current_station_name());
    
   /* trackbar */
    btn_add_track.set_stock_id(Gtk::Stock::APPLY);
    trackbar.append(btn_add_track, sigc::mem_fun(*this, &MainWindow::on_btn_add_track));
    trackbar.append(sep3);
    trackbar.append(lbl_track);
    lbl_track.set_text("...");
        
    vbox.set_orientation(Gtk::ORIENTATION_VERTICAL);
    vbox.pack_start (mainbar, Gtk::PACK_SHRINK, 0);
    vbox.pack_start (trackbar, Gtk::PACK_SHRINK, 0);
    
   /* UiTextTreeView::tview - initialized by init_trackview() */
    vbox.pack_start(*(tview.get_scrolled_window()), Gtk::PACK_EXPAND_WIDGET, 0);
    
   /* tview control */
    hbox_tviewcontrol.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    btn_tview_delete.signal_clicked().connect(sigc::mem_fun(*this,
                                             &MainWindow::on_btn_tview_delete));
    hbox_tviewcontrol.pack_end(btn_tview_delete, Gtk::PACK_SHRINK, 0);
    
    vbox.pack_start(hbox_tviewcontrol, Gtk::PACK_SHRINK, 0);
    
    add(vbox);
    show_all();
}

bool
MainWindow::init_trackview()
{
   /* init gui elements */
    tview.init();
    tview.set_tracks_db(controller.tracks.m_tracks_db);
    
    tview.signal_open_xdg_url.connect(sigc::mem_fun(controller, 
                                      &SessionControl::open_url_xdg));
    return true;
}

void
MainWindow::on_btn_settings()
{
    // TODO
    // Create Settings-Dialog
    // +choose player interface
    // +choose programs working path (maybe it will be used with a cloud-service)
    // +import foreign station-db
    // +import foreign tracks-db
    // +Skins?
}

void
MainWindow::on_btn_load_station()
{
    gats::s_station s;
    if (controller.stations.run_stations_dialog(s))
    {
        std::stringstream ss;
        std::string sep = " | ";
        ss << s.name     << sep
           << s.homepage << sep
           << s.genre    << sep
           << s.country  << sep
           << s.city;
        set_lbl_station(ss.str());
        
        set_lbl_track("...");
        
        controller.set_current_station(s);
        controller.player_play();
        
        std::cout << "Info: " << __PRETTY_FUNCTION__
                  << "load station:"
                  << s.name
                  << std::endl;
    }
    else
    {
        std::cerr << "WARN:" << __PRETTY_FUNCTION__
                  << ":Stations::run_stations_dialog aborted or error"
                  << std::endl;
    }
}


void
MainWindow::set_lbl_station(const std::string& station)
{
    lbl_station.set_text(station);
}

void
MainWindow::set_lbl_track(const std::string& track)
{
    lbl_track.set_text(track);
}

void
MainWindow::on_btn_play()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
   // activate
    if (!is_playing)
    {
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
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    btn_play.set_stock_id(Gtk::Stock::MEDIA_PLAY);
    is_playing = false;
    is_paused = false;
    controller.player_stop();
    lbl_track.set_text("...");
}

void
MainWindow::on_volume_changed(double vol)
{
    controller.set_volume(vol);
}

void
MainWindow::on_show_station_menu()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
}

void
MainWindow::on_menustation_show()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    on_btn_load_station();
}

void
MainWindow::on_menustation_add()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    auto builder = Gtk::Builder::create();
    try
    {
        builder->add_from_file("src/uidialognewstation.ui");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr << "  FileError: " << ex.what() << std::endl;
        return;
    }
    catch(const Glib::MarkupError& ex)
    {
        std::cerr << "  MarkupError: " << ex.what() << std::endl;
        return;
    }
    catch(const Gtk::BuilderError& ex)
    {
        std::cerr << "  BuilderError: " << ex.what() << std::endl;
        return;
    }
    
    gats::UiDialogNewStation *dlg = nullptr;
    builder->get_widget_derived("dlg_new_station", dlg);
    if ( dlg )
    {
        gats::s_station s;
        if ( dlg->exec(s) )
        {
            s.datetime = controller.get_datetime();
            controller.stations.add_new_station(s);
        }
    }
    else
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ":cannot exec New Station Dialog"
                << std::endl;
    }
    
    delete dlg;
}

void
MainWindow::on_station_new_pls()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    // TODO: add local station-playlist
}

void
MainWindow::on_event_new_track(std::string track)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << ":" << track << std::endl;
    lbl_track.set_text(track);
}

void
MainWindow::on_btn_add_track() 
{
    gats::s_track t;
    controller.init_with_current_track(t);
    controller.tracks.add_track(t);
    tview.add_track(t);
    //scrolled_window.get_vadjustment()->set_value(-100);
//    double value = scrolled_window.get_vadjustment()->get_value();
//    double pg_sz = scrolled_window.get_vadjustment()->get_page_size();
//    double pg_inc = scrolled_window.get_vadjustment()->get_page_increment();
    
    scrolled_window.get_vadjustment()->set_value(-3000);
    
    std::cout << "Info:" << __PRETTY_FUNCTION__
              << ":track added:"<< t.added_datetime << ":"
              << "id:" << t.id << ":"
              << t.artist << " - " << t.title
              << std::endl;
}

void
MainWindow::on_btn_tview_delete()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << "remove tracks:" << std::endl;
    
    auto track_ids = tview.delete_rows();
    for (const auto &id : track_ids)
    {
        std::cout << " "  << id;
        controller.tracks.remove_track(id);
    }
    
    std::cout << std::endl;
}
