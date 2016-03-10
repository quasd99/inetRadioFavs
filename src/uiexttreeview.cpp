#include "uiexttreeview.h"

using namespace gats;
 
UiExtTreeview::UiExtTreeview()
{
}

bool
UiExtTreeview::init()
{
    // treeview
    tree_model = Gtk::TreeStore::create(cols);
    
    set_model(tree_model);
    append_column("", cols.col_btn_discogs);
    append_column("Artist", cols.col_artist);
    append_column("Title", cols.col_title);
    append_column("Added", cols.col_datetime);
    append_column("Station", cols.col_station);
    set_margin_bottom(10);
    set_activate_on_single_click(true);
//    tview.signal_row_activated().connect(sigc::mem_fun(this, 
//                                         &MainWindow::slot_row_activated));
    //scrolled_window.add(tview);
    
    // popup
    auto item = Gtk::manage(new Gtk::MenuItem("_Edit", true));
    item->signal_activate().connect(
      sigc::mem_fun(*this, &UiExtTreeview::on_menu_file_popup_generic) );
    
    item->show();
    popup_title.append(*item);
    
    
    return true;
}

void
UiExtTreeview::add_track(const s_track& t)
{
    std::cout << __PRETTY_FUNCTION__ << ":" << std::endl;
    Gtk::TreeModel::Row row = *(tree_model->append());
    
    auto pbuf_dc = Gdk::Pixbuf::create_from_file("/home/fra/discogs.png");    
    row[cols.col_btn_discogs] = pbuf_dc;
    row[cols.col_id]        = t.id;
    row[cols.col_artist]    = t.artist;
    row[cols.col_title]     = t.title;
    row[cols.col_datetime]  = t.datetime;
    row[cols.col_station]   = t.station;
    
    resize_children();

}


bool
UiExtTreeview::on_button_press_event(GdkEventButton* button_event)
{
    bool return_value = false;

    //Call base class, to allow normal handling,
    //such as allowing the row to be selected by the right-click:
    return_value = Gtk::TreeView::on_button_press_event(button_event);

    //Then do our custom stuff:
    if( (button_event->type == GDK_BUTTON_PRESS) && (button_event->button == 3) )
    {
      popup_title.popup(button_event->button, button_event->time);
    }

    return return_value;
}

void UiExtTreeview::on_menu_file_popup_generic()
{
  std::cout << "A popup menu item was selected." << std::endl;

  auto refSelection = get_selection();
  if(refSelection)
  {
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    if(iter)
    {
        // row by id
        int id = (*iter)[cols.col_id];
        // column
        
        std::cout << "  Selected ID=" << id << std::endl;
    }
  }
}

void
UiExtTreeview::slot_row_activated(const Gtk::TreePath& path,
                                  Gtk::TreeViewColumn* column)
{

}
