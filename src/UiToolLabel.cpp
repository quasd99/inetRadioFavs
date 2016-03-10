/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UiToolLabel.cpp
 * Author: fra
 * 
 * Created on 3. März 2016, 16:18
 */

#include <gtkmm-3.0/gtkmm/label.h>

#include "UiToolLabel.h"

using namespace gats;

UiToolLabel::UiToolLabel()
{
    frame.set_border_width(1);
    frame.add_label("Hallo");
    vbox.set_orientation(Gtk::ORIENTATION_VERTICAL);
    vbox.pack_start (lbl, Gtk::PACK_EXPAND_WIDGET, 0);
    add(vbox);
    show_all();
}

void
UiToolLabel::set_text(const std::string &text)
{
    lbl.set_text(text);
}
