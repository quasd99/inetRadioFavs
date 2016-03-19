#include "uiexttreeview.h"

using namespace gats;
 
UiExtTreeview::UiExtTreeview()
{
}

bool
UiExtTreeview::init()
{
    // settings
    set_activate_on_single_click(true);
    set_border_width(1);
    set_enable_search(true);
    set_grid_lines(Gtk::TREE_VIEW_GRID_LINES_BOTH);
    set_headers_clickable(true);
    set_rubber_banding(true);
    set_rules_hint(true);
    
    // treeview
    tree_model = Gtk::TreeStore::create(cols);
    set_model(tree_model);
    
    append_column("DC", cols.col_btn_discogs);
    append_column("YT", cols.col_btn_youtube);
    append_column("Artist", cols.col_artist);
    append_column("Title", cols.col_title);
    append_column("Added", cols.col_added_datetime);
    append_column("Station", cols.col_station);
    
    set_activate_on_single_click(true);
    signal_row_activated().connect( sigc::mem_fun(this, 
                                           &UiExtTreeview::slot_row_activated));
    
    // popup
    auto item = Gtk::manage(new Gtk::MenuItem("_Edit", true));
    item->signal_activate().connect( sigc::mem_fun(*this, 
                                   &UiExtTreeview::on_menu_file_popup_generic));
    
    item->show();
    popup_title.append(*item);
    
    scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scrolled_window->add(*this);
    
    return true;
}

void
UiExtTreeview::add_track(const s_track& t)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ 
              << ":" << t.artist << " - " << t.title
              << std::endl;
    
    Gtk::TreeModel::Row row = *(tree_model->prepend());
    //Gtk::TreeModel::Row row = *(tree_model->append());
    
    auto pbuf_dc = Gdk::Pixbuf::create_from_file("/home/fra/discogs.png");
    auto pbuf_yt = Gdk::Pixbuf::create_from_file("/home/fra/youtube.png");
    row[cols.col_btn_discogs]    = pbuf_dc;
    row[cols.col_btn_youtube]    = pbuf_yt;
    row[cols.col_id]             = t.id;
    row[cols.col_artist]         = t.artist;
    row[cols.col_title]          = t.title;
    row[cols.col_added_datetime] = t.added_datetime;
    row[cols.col_station]        = t.station;
    
    //scrolled_window->get_vadjustment()->set_value(-24);
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
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    auto iter = tree_model->get_iter(path);
    unsigned int track_id = 0;
    std::string artist;
    std::string title;
    if (iter)
    {
        track_id = (*iter)[cols.col_id];
        artist = (*iter)[cols.col_artist];
        std::replace( artist.begin(), artist.end(), ' ', '+');
        std::replace( artist.begin(), artist.end(), '&', '+');
        title = (*iter)[cols.col_title];
        std::replace( title.begin(), title.end(), ' ', '+');
        std::replace( title.begin(), title.end(), '&', '+');
    }
    
    std::string column_title = column->get_title();
    std::stringstream ss_url;
    ss_url << "https://";
    if ( column_title == "DC" )
    {
        std::cout << "Info:" << __PRETTY_FUNCTION__
                << ":track_id:" << track_id
                << ":discogs"
                << std::endl;
        
        ss_url << "//www.discogs.com/search/?q=";
        ss_url << artist << "+" << title << std::endl;
        signal_open_xdg_url.emit(ss_url.str());
    }
    else if ( column_title == "YT" )
    {
        std::cout << "Info:" << __PRETTY_FUNCTION__
                << ":track_id:" << track_id
                << ":youtube"
                << std::endl;

        ss_url << "www.youtube.com/results?search_query=";
        ss_url << artist << "+" << title << std::endl;
        signal_open_xdg_url.emit(ss_url.str());
    }
    
}
