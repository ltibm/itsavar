#ifndef _SETTING_
#define _SETTING_
#include <unordered_map>
std::unordered_map<char*, char*> EnglishTable = {
	{"CONFIG_LOADED", "Config file successfully loaded"},
	{"CONFIG_FAIL", "Config file is failed to load"},
	{"MODULE_NAME","Anti DDOS Module"},
	{"MODULE_LOADED","%s is loaded"},
	{"MODULE_LOAD_FAIL","Failed to Load %s"},
	{"IP_PACKET_BLOCK","This ip address is temporary blocked due packet attacking: %s"},
	{"IP_PACKET_BLOCK_DIFF","This ip adress temporary blocked due its sends continuous differents packet: %s"},
	{"IP_PACKET_BLOCK_SAME","This ip adress temporary blocked due its sends continuous same packet: %s"},
	{"IP_PACKET_BLOCK_DOUBT","This ip adress is blocked due doubt to attack(type: %i): %s"},
	{"PACKET", "Packet"},
	{"CMD_LIST_NOIP", "There is no blocked ip's"},
	{"CMD_LIST_HEADER", "Blocked IP Counts: %i"},
	{"CMD_LIST_CONTENT", "%i: %s, Banned Time: %s, Index: %i"},
	{"CMD_CLEAR_OK", "All blocked ip addresses are removed"},
	{"USAGE", "Usage"},
	{"CMD_REMOVE_INVALID", "Invalid index or IpAddress"},
	{"CMD_REMOVE_OK", "Successfully removed"},
	{"CMD_INFO_VERSION", "Version"},
	{"CMD_INFO_TOTAL_BLOCK", "Total blocked attack for current session: %i"},
	{"CMD_INFO_CUR_BLOCK", "Currently blocked IPAddress count: %i"},
	{"CMD_INFO_ALLOWED_IPCOUNT", "Allowed IP Addresses: %i"},
	{"CMD_INFO_LOADED_BANNED", "Loaded Banned Packets: %i"},
	{"CMD_INFO_LOADED_ALLOWED", "Loaded Allowed Packets: %i"},
	{"CMD_INFO_LOADED_CHECKS", "Loaded Packets to Check: %i"},
	{"COMMANDS", "Commands"},
	{"CMD_INFO", "Show modelue information"},
	{"CMD_LIST", "List currenty blocked ip address"},
	{"CMD_CLEAR", "Clear all currently blcoked ip address"},
	{"CMD_REMOVE", "index|ip:Remove single blocked ip adress from list"},
};
std::unordered_map<char*, char*> TurkishTable = {
	{"CONFIG_LOADED", "Ayar dosyasi basariyle yuklendi"},
	{"CONFIG_FAIL", "Ayar dosyasi yuklemesi basarisiz oldu"},
	{"MODULE_NAME","Anti DDOS Moldulu"},
	{"MODULE_LOADED","%s yuklendi"},
	{"MODULE_LOAD_FAIL","%s yuklemesi basarisiz oldu"},
	{"IP_PACKET_BLOCK","Bu IP adresi paket saldirisi sebebiyle bloklandi: %s"},
	{"IP_PACKET_BLOCK_DIFF","Bu IP adresi farkli paketleri surekli gondermesi sebebiyle gecici olarak bloklandi: %s"},
	{"IP_PACKET_BLOCK_SAME","Bu IP adresi ayni paketleri surekli gondermesi sebebiyle gecici olarak bloklandi: %s"},
	{"IP_PACKET_BLOCK_DOUBT","Bu IP adresi saldiri suphesi sebebiyle bloklandi(tip: %i): %s"},
	{"PACKET", "Paket"},
	{"CMD_LIST_NOIP", "Bloklanmis IP adresi yok"},
	{"CMD_LIST_HEADER", "Bloklanan IP: %i"},
	{"CMD_LIST_CONTENT", "%i: %s, Yasaklandi: %s, Index: %i"},
	{"CMD_CLEAR_OK", "Tum bloklanan IP adresleri temizlendi"},
	{"USAGE", "Kullanim"},
	{"CMD_REMOVE_INVALID", "Gecersiz Index veya IPAdresi"},
	{"CMD_REMOVE_OK", "Basariyla silindi"},
	{"CMD_INFO_VERSION", "Surum"},
	{"CMD_INFO_TOTAL_BLOCK", "Bu oturumda bloklanan atak sayisi: %i"},
	{"CMD_INFO_CUR_BLOCK", "Mevcut bloklanan IP' ler: %i"},
	{"CMD_INFO_ALLOWED_IPCOUNT", "Izin Verilen IP Sayisi: %i"},
	{"CMD_INFO_LOADED_BANNED", "Yasakli Paketler Yuklendi: %i"},
	{"CMD_INFO_LOADED_ALLOWED", "Izin Verilem Paketler Yuklendi: %i"},
	{"CMD_INFO_LOADED_CHECKS", "Kontrol Edilecek Paketler Yuklendi: %i"},
	{"COMMANDS", "Komutlar"},
	{"CMD_INFO", "Modul bilgisini gosterir"},
	{"CMD_LIST", "Mevcut bloklanan IP adreslerini gosterir"},
	{"CMD_CLEAR", "Mevcut bloklanmis IP adreslerini temizler"},
	{"CMD_REMOVE", "index|ip:Tekil bloklanmis IP adresini siler"},
};

class Settings
{
public:
	Settings();
	char* DateFormat;
	std::unordered_map<char*, char*> LangTable;
	void SetLanguage(const char*);
	char* GetText(char* name)
	{
		return this->LangTable[name];
	}
	char* GetTextFmt(char* name, ...)
	{
		char* returned = this->LangTable[name];
		if (returned)
		{
			char* buffer = new char[500];
			va_list argptr;
			
			va_start(argptr, name);
			vsnprintf(buffer, 499, returned, argptr);
			va_end(argptr);
			return buffer;

		}
		return "";
	}
};

Settings::Settings()
{
	this->DateFormat = "%d.%m.%Y %H:%M:%S";
	this->SetLanguage("EN");
}
void Settings::SetLanguage(const char* lang)
{
	if (strcmpi(lang, "TR") == 0)
	{
		this->LangTable = TurkishTable;
	}
	else
	{
		this->LangTable = EnglishTable;
	}
}


#endif // !_SETTING_

