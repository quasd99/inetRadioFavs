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
        void set_tracks_db(std::map<unsigned int, s_track> &list);
        std::vector<unsigned int> delete_rows(); 
        
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
                add(id);
                add(btn_discogs);
                add(btn_youtube);
                add(artist);
                add(title);
                add(added_datetime);
                add(station);
            }
            ~Columns() {}
            Gtk::TreeModelColumn<unsigned int> id; // hidden
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> btn_discogs;
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> btn_youtube;
            //<Gtk::Button*> col_btn_youtube;
            Gtk::TreeModelColumn<std::string> artist;
            Gtk::TreeModelColumn<std::string> title;
            Gtk::TreeModelColumn<std::string> added_datetime;
            Gtk::TreeModelColumn<std::string> station;
        };
        
        //Gtk::TreeView  tview;
        Columns columns;
        Glib::RefPtr<Gtk::TreeStore> treemodel;
        Gtk::Menu popup_title;
        Gtk::ScrolledWindow* scrolled_window;
        
        // slots
        void slot_row_activated(const Gtk::TreeModel::Path& path, 
                                Gtk::TreeViewColumn* column);
    };

} // namespace gats
 
#endif // UIEXTTREEVIEW_H
