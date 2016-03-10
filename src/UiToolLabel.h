/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UiToolLabel.h
 * Author: fra
 *
 * Created on 3. März 2016, 16:18
 */

#ifndef UITOOLLABEL_H
#define UITOOLLABEL_H

#include <gtkmm-3.0/gtkmm.h>

namespace gats
{
    class UiToolLabel : public Gtk::ToolItem
    {
        Gtk::Frame frame;
        Gtk::Box vbox;
        Gtk::Label lbl;
    public:
        UiToolLabel();
        void set_text(const std::string &text);
    private:

    };
}
#endif /* UITOOLLABEL_H */

