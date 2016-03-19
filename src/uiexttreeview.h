#ifndef UIEXTTREEVIEW_H
#define UIEXTTREEVIEW_H

#include <gtkmm-3.0/gtkmm.h>
#include "SessionControl.h"

namespace gats
{
 
    class UiExtTreeview : public Gtk::TreeView
    {
    public:
        UiExtTreeview();
        bool init();
        void add_track(const s_track &t);
        
        
        Gtk::ScrolledWindow* get_scrolled_window() { return scrolled_window; };
        
        sigc::signal<void, std::string> signal_open_xdg_url;
    protected:
        // Override Signal handler:
        // Alternatively, use signal_button_press_event().connect_notify()
        bool on_button_press_event(GdkEventButton* button_event) override;

        //Signal handler for popup menu items:
        void on_menu_file_popup_generic();
        
        unsigned int selected_id;
    
        // trackview
        class Columns : public Gtk::TreeModel::ColumnRecord
        {
        public:
            Columns() {
                add(col_id);
                add(col_btn_discogs);
                add(col_btn_youtube);
                add(col_artist);
                add(col_title);
                add(col_added_datetime);
                add(col_station);
            }
            ~Columns() {}
            Gtk::TreeModelColumn<unsigned int> col_id; // hidden
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> col_btn_discogs;
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> col_btn_youtube;
            //<Gtk::Button*> col_btn_youtube;
            Gtk::TreeModelColumn<std::string> col_artist;
            Gtk::TreeModelColumn<std::string> col_title;
            Gtk::TreeModelColumn<std::string> col_added_datetime;
            Gtk::TreeModelColumn<std::string> col_station;        
        };
        
        //Gtk::TreeView  tview;
        Columns cols;
        Glib::RefPtr<Gtk::TreeStore> tree_model;
        Gtk::Menu popup_title;
        Gtk::ScrolledWindow* scrolled_window;
        
        // slots
        void slot_row_activated(const Gtk::TreeModel::Path& path, 
                                Gtk::TreeViewColumn* column);
    };

} // namespace gats
 
#endif // UIEXTTREEVIEW_H
