#include "uidialognewstation.h"

#include <iostream>

using namespace gats;
 
UiDialogNewStation::UiDialogNewStation(BaseObjectType* cobject,
                                       const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Dialog(cobject),
  builder(refGlade)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    set_title("Add new Radio Station");
    set_default_size(800, 160);
    
    builder->get_widget("cmb_selection", cmb_selection);
    if ( !cmb_selection )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
               << ":error init ui:cmb_selection"
               << std::endl;
        return;
    }
    
    builder->get_widget("entry_name", entry_name);
    if ( !entry_name )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
               << ":error init ui:entry_name"
               << std::endl;
        return;
    }

    builder->get_widget("entry_address", entry_address);
    if ( !entry_name )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
               << ":error init ui:entry_address"
               << std::endl;
        return;
    }
    
    builder->get_widget("searchentry_genre", searchentry_genre);
    if ( !searchentry_genre )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
               << ":error init ui:searchentry_genre"
               << std::endl;
        return;
    }
    
    builder->get_widget("entry_homepage", entry_homepage);
    if ( !entry_homepage )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
               << ":error init ui:entry_homepage"
               << std::endl;
        return;
    }
    
    builder->get_widget("entry_country", entry_country);
    if ( !entry_country )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
               << ":error init ui:entry_country"
               << std::endl;
        return;
    }
    
    builder->get_widget("entry_city", entry_city);
    if ( !entry_city )
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
               << ":error init ui:entry_city"
               << std::endl;
        return;
    }

    
    Gdk::RGBA col;
    col.set_rgba(1, 0, 0, 0.7);
    entry_homepage->override_background_color(col, Gtk::STATE_FLAG_NORMAL);
     
    add_button(Gtk::Stock::CANCEL, -1);
    
    // add a button NOT via Gtk::Dialog::add_button to avoid that the signal
    // Gtk::Dialog::signal_response(int) is called the terminates Gtk::Dialog::run
    // Do validation: on_btn_add_clicked -> then call Gtk::Dialog::response(int)
    btn_add.set_label("Add");
    get_action_area()->pack_start(btn_add);
    btn_add.show();
    btn_add.signal_clicked().connect(sigc::mem_fun(*this,
                                      &UiDialogNewStation::on_btn_add_clicked));
}

UiDialogNewStation::~UiDialogNewStation() {}

bool
UiDialogNewStation::exec(s_station& s)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    int status = run();
    
    switch (status)
    {
        case 0:
            // validation through on_btn_add_clicked()
            set_s_station_userdata(s);
            std::cout << "  add:" << s.name << ":" << s.address << std::endl;
            return true;
            break;
        case -1:
        default:
            std::cout << "  user abortion" << std::endl;
            return false;
            break;
    }   
}

void
UiDialogNewStation::on_btn_add_clicked()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    bool address = entry_address->get_text().empty();
    bool name = entry_name->get_text().empty();
    
    if ( !address && !name )
    {
        // return 0 to this::exec() and closes this dialog
        response(0);
    }
    else
    {
        // show warning dialog
        std::string msg = "Station-Name and/or Station-Address not set";
        Gtk::MessageDialog dlg_warn(msg, false, Gtk::MESSAGE_INFO,
                               Gtk::BUTTONS_OK, true);
        dlg_warn.run();
    }
}

void
UiDialogNewStation::set_s_station_userdata(s_station& s)
{
    s.name = entry_name->get_text();
    s.address = entry_address->get_text();
    
    std::string value = entry_homepage->get_text();
    if ( !value.empty() )
    {
        s.homepage = value;
    }
    
    value = searchentry_genre->get_text();
    if ( !value.empty() )
    {
        s.genre = value;
    }

    value = entry_country->get_text();
    if ( !value.empty() )
    {
        s.country = value;
    }

    value = entry_city->get_text();
    if ( !value.empty() )
    {
        s.city = value;
    }
}

