bin_PROGRAMS = iradiofavs
iradiofavs_SOURCES = src/iradiofavs.cpp src/MainApplication.h src/MainApplication.cpp src/MainWindow.h src/MainWindow.cpp VlcMediaPlayer2_proxy.h src/VlcDBusInterface.h src/VlcDBusInterface.cpp src/SessionControl.h src/SessionControl.cpp src/UiToolLabel.h src/UiToolLabel.cpp src/UiToolVolumeButton.h src/UiToolVolumeButton.cpp src/DialogStations.h src/DialogStations.cpp src/uiexttreeview.h src/uiexttreeview.cpp src/stations.h src/stations.cpp src/tracks.h src/tracks.cpp src/uidialognewstation.h src/uidialognewstation.cpp src/file.h src/file.cpp
iradiofavs_CPPFLAGS = -Wall -O3 -std=c++11 -pedantic -DDEBUG `pkg-config --cflags gtkmm-3.0 dbus-c++-1 glibmm-2.4 jsoncpp`
iradiofavs_LDFLAGS = `pkg-config --libs gtkmm-3.0 dbus-c++-1 glibmm-2.4 jsoncpp` -lpthread -liberty -lboost_system -lboost_filesystem
# -liberty (asprintf,strdup)

# m4 convention
ACLOCAL_AMFLAGS = -I m4 --install
# autoreconf 2.68 workaround: touch m4/NOTES ; git add m4/NOTES
EXTRA_DIST = m4/NOTES


