/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MainApplication.cpp
 * Author: fra
 * 
 * Created on February 20, 2016, 2:22 PM
 */

#include <iostream>
#include "MainApplication.h"
#include "MainWindow.h"

MainApplication::MainApplication()
: Gtk::Application("org.gats.iradiofavs")
{
    Glib::set_application_name("IRadioFavs");
}

Glib::RefPtr<class MainApplication>
MainApplication::create()
{
    return Glib::RefPtr<MainApplication>(new MainApplication());
}

void 
MainApplication::on_startup()
{
//Call the base class's implementation:
  Gtk::Application::on_startup();

  //Create actions for menus and toolbars.
  //We can use add_action() because Gtk::Application derives from Gio::ActionMap.

  //File|New sub menu:
/*
  add_action("newfoo",
    sigc::mem_fun(*this, &MainApplication::on_menu_file_new_generic));

  add_action("newgoo",
    sigc::mem_fun(*this, &MainApplication::on_menu_file_new_generic));
*/  
  //File menu:
  add_action("quit", sigc::mem_fun(*this, &MainApplication::on_menu_file_quit));

  //Help menu:
  add_action("about", sigc::mem_fun(*this, &MainApplication::on_menu_help_about));
}

void
MainApplication::on_activate()
{
  //std::cout << "debug1: " << G_STRFUNC << std::endl;
  // The application has been started, so let's show a window.
  // A real application might want to reuse this window in on_open(),
  // when asked to open a file, if no changes have been made yet.
  create_window();
}

void
MainApplication::create_window()
{
  mainwindow = new MainWindow();

  //Make sure that the application runs for as long this window is still open:
  add_window(*mainwindow);

  //Delete the window when it is hidden.
  //That's enough for this simple example.
  mainwindow->signal_hide().connect(sigc::bind<Gtk::Window*>(
    sigc::mem_fun(*this, &MainApplication::on_window_hide), mainwindow));
  
  mainwindow->show_all();
}

void MainApplication::on_window_hide(Gtk::Window* window)
{
  delete window;
}

void MainApplication::on_menu_file_quit()
{
  std::cout << G_STRFUNC << std::endl;
  quit(); // Not really necessary, when Gtk::Widget::hide() is called.

  // Gio::Application::quit() will make Gio::Application::run() return,
  // but it's a crude way of ending the program. The window is not removed
  // from the application. Neither the window's nor the application's
  // destructors will be called, because there will be remaining reference
  // counts in both of them. If we want the destructors to be called, we
  // must remove the window from the application. One way of doing this
  // is to hide the window.
  std::vector<Gtk::Window*> windows = get_windows();
  if (windows.size() > 0)
    windows[0]->hide(); // In this simple case, we know there is only one window.
}

void MainApplication::on_menu_help_about()
{
  std::cout << "App|Help|About was selected." << std::endl;
}
