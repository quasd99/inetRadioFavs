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
    get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
    
    // treeview
    treemodel = Gtk::TreeStore::create(columns);
    set_model(treemodel);
    
    append_column("DC", columns.btn_discogs);
    append_column("YT", columns.btn_youtube);
    append_column("Artist", columns.artist);
    append_column("Title", columns.title);
    append_column("Added", columns.added_datetime);
    append_column("Station", columns.station);
    
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
    
    Gtk::TreeModel::Row row = *(treemodel->prepend());
    
    auto pbuf_dc = Gdk::Pixbuf::create_from_file("/home/fra/discogs.png");
    auto pbuf_yt = Gdk::Pixbuf::create_from_file("/home/fra/youtube.png");
    row[columns.btn_discogs]    = pbuf_dc;
    row[columns.btn_youtube]    = pbuf_yt;
    row[columns.id]             = t.id;
    row[columns.artist]         = t.artist;
    row[columns.title]          = t.title;
    row[columns.added_datetime] = t.added_datetime;
    row[columns.station]        = t.station;
    
    // scroll automatically to new row
    scroll_to_row(Gtk::TreePath(row));
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
        int id = (*iter)[columns.id];
        // column
        
        std::cout << "  Selected ID=" << id << std::endl;
    }
  }
}

void
UiExtTreeview::slot_row_activated(const Gtk::TreePath& path,
                                  Gtk::TreeViewColumn* column)
{   
    auto iter = treemodel->get_iter(path);
    unsigned int track_id = 0;
    std::string artist;
    std::string title;
    if (iter)
    {
        track_id = (*iter)[columns.id];
        artist = (*iter)[columns.artist];
        std::replace( artist.begin(), artist.end(), ' ', '+');
        std::replace( artist.begin(), artist.end(), '&', '+');
        title = (*iter)[columns.title];
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
    std::cout << "Info:" << __PRETTY_FUNCTION__ << ":" << track_id << std::endl;
}

void
UiExtTreeview::set_tracks_db(std::map<unsigned int, s_track>& list)
{
    // delete old data
    treemodel->clear();
    
    // detach from tview
    Glib::RefPtr<Gtk::TreeModel> none;
    this->set_model(none);
    
    // load data
    for (const auto &track : list)
    {
        Gtk::TreeModel::Row row = *(treemodel->prepend());

        auto pbuf_dc = Gdk::Pixbuf::create_from_file("/home/fra/discogs.png");
        auto pbuf_yt = Gdk::Pixbuf::create_from_file("/home/fra/youtube.png");
        row[columns.btn_discogs]    = pbuf_dc;
        row[columns.btn_youtube]    = pbuf_yt;
        row[columns.id]              = track.first;
        row[columns.artist]         = track.second.artist;
        row[columns.title]          = track.second.title;
        row[columns.added_datetime] = track.second.added_datetime;
        row[columns.station]        = track.second.station;
    }
        
    // attach model again
    this->set_model(treemodel);
}


std::vector<unsigned int>
UiExtTreeview::delete_rows()
{
    // return vector
    std::vector<unsigned int> ret_track_ids;
    // row references
    std::vector<Gtk::TreeModel::RowReference> rows;
    
    auto selection = this->get_selection();
    for (auto rowpath : selection->get_selected_rows())
    {
        // save reference
        rows.emplace_back(Gtk::TreeModel::RowReference(treemodel, rowpath));
        
        // get the track id from the row
        auto it = treemodel->get_iter(rowpath);
        if (it)
        {
            ret_track_ids.emplace_back((*it)[columns.id]);
        }
    }
    
    // remove the row-references from treemodel
    for (auto &it : rows)
    {
        auto treeiter = treemodel->get_iter(it.get_path());
        if (treeiter)
        {
            treemodel->erase(treeiter);
        }   
    }
    
    return std::move(ret_track_ids);
}
