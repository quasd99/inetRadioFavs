/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MainApplication.h
 * Author: fra
 *
 * Created on February 20, 2016, 2:22 PM
 */

#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include <gtkmm-3.0/gtkmm.h>
#include <sigc++-2.0/sigc++/sigc++.h>
#include "MainWindow.h"

class MainApplication : public Gtk::Application
{
protected:
    MainApplication();
public:
    static Glib::RefPtr<class MainApplication> create();
    MainWindow *mainwindow;
    
protected:
    // Overrides of default signal handlers
    void on_startup() override;
    void on_activate() override;
private:
    void create_window();
    
    void on_window_hide(Gtk::Window *window);
    void on_menu_file_quit();
    void on_menu_help_about();
};

#endif /* MAINAPPLICATION_H */

