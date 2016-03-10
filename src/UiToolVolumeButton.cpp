/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UiToolVolumeButton.cpp
 * Author: fra
 * 
 * Created on 3. März 2016, 15:18
 */

#include "UiToolVolumeButton.h"
#include "MainApplication.h"

using namespace gats;

UiToolVolumeButton::UiToolVolumeButton()
{
    add(btn_vol);
    show_all();
}

void
UiToolVolumeButton::set_volume(const double& vol)
{
    btn_vol.set_value(vol);
}
