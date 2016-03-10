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

    protected:
        // Override Signal handler:
        // Alternatively, use signal_button_press_event().connect_notify()
        bool on_button_press_event(GdkEventButton* button_event) override;

        //Signal handler for popup menu items:
        void on_menu_file_popup_generic();
        
        unsigned int selected_id;
        void slot_row_activated(const Gtk::TreePath &path,
                                Gtk::TreeViewColumn *column);
    
        // trackview
        class Columns : public Gtk::TreeModel::ColumnRecord
        {
        public:
            Columns() {
                add(col_id);
                add(col_btn_discogs);
                add(col_artist);
                add(col_title);
                add(col_datetime);
                add(col_station);

            }
            ~Columns() {}
            Gtk::TreeModelColumn<unsigned int> col_id; // hidden
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> col_btn_discogs;
            Gtk::TreeModelColumn<Glib::ustring> col_artist;
            Gtk::TreeModelColumn<Glib::ustring> col_title;
            Gtk::TreeModelColumn<Glib::ustring> col_datetime;
            Gtk::TreeModelColumn<Glib::ustring> col_station;        
        };
        
        //Gtk::TreeView  tview;
        Columns cols;
        Glib::RefPtr<Gtk::TreeStore> tree_model;
        
        Gtk::Menu popup_title;
    };

} // namespace gats
 
#endif // UIEXTTREEVIEW_H
