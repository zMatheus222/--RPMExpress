#include "default_functions.hpp"
#include "mysql_functions.hpp"

using namespace std;
using namespace TXT_Style;

std::string DownloaderBase;

void rpm_downloader() {

   Exlog("INFO", AZUL + __func__, VERDE + "Started!");

   DownloaderBase += string("ACC-SKINSET: ") + MySQL::PickLink("ACC-SKINSET", "direct_download_link") + "\n";
   DownloaderBase += string("ACC-SKINSET-TITLE: ") + MySQL::PickLink("ACC-SKINSET", "item_title") + "\n";
   DownloaderBase += string("ACC-SKINSET-DESCRIPTION: ") + MySQL::PickLink("ACC-SKINSET", "item_description") + "\n";

   DownloaderBase += string("AC1-EVENT-MODPACK: ") + MySQL::PickLink("AC1-EVENT-MODPACK", "direct_download_link") + "\n";
   DownloaderBase += string("AC1-EVENT-TITLE: ") + MySQL::PickLink("AC1-EVENT-MODPACK", "item_title") + "\n";
   DownloaderBase += string("AC1-EVENT-DESCRIPTION: ") + MySQL::PickLink("AC1-EVENT-MODPACK", "item_description");

   Exlog("INFO", AZUL + __func__, VERDE + "DownloaderBase update concluded");

   Directories::RPMDownloaderOnEnd = true;

}