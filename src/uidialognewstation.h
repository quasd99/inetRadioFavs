#ifndef UIDIALOGNEWSTATION_H
#define UIDIALOGNEWSTATION_H

#include <stdio.h>
#include <gtkmm-3.0/gtkmm.h>

#include "stations.h"

namespace gats
{
 
class UiDialogNewStation : public Gtk::Dialog
{   
    /**
     * @brief Called by slot-method on_btn_add_clicked. Validates the user-data
     * and closes the dialog with response_id(0);
     */
    //void validate();
    void set_s_station_userdata(s_station &s);
    public:
    UiDialogNewStation(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& refGlade);
    virtual ~UiDialogNewStation();
    
    /**
     * @brief Method to prepare and call Gtk::Dialog::run()
     */
    bool exec(s_station& s);
    
    /* slots */
    void on_btn_add_clicked();
    
    protected:
    Glib::RefPtr<Gtk::Builder> builder;
    
    /* glade ui reference-pointer */
    Gtk::ComboBox    *cmb_selection;
    Gtk::Entry       *entry_name;
    Gtk::Entry       *entry_address;
    Gtk::Entry       *entry_homepage;
    Gtk::Entry       *entry_country;
    Gtk::Entry       *entry_city;
    Gtk::SearchEntry *searchentry_genre;
    
    Gtk::Button btn_add;
};

} // namespace gats
 
#endif // UIDIALOGNEWSTATION_H
