/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UiToolVolumeButton.h
 * Author: fra
 *
 * Created on 3. März 2016, 15:18
 */

#ifndef UITOOLVOLUMEBUTTON_H
#define UITOOLVOLUMEBUTTON_H

#include <gtkmm-3.0/gtkmm.h>

namespace gats
{
    class UiToolVolumeButton : public Gtk::ToolItem
    {
    public:
        Gtk::VolumeButton btn_vol;
        UiToolVolumeButton();
        void set_volume(const double &vol);
    private:

    };
}
#endif /* UITOOLVOLUMEBUTTON_H */

