#include "default_functions.hpp"
#include "mysql_functions.hpp"
#include "AnalysisTools.hpp"
#include "DiscordBot.hpp"
#include "RankingTools.hpp"

#include <iostream>

using namespace TXT_Style;
using namespace std;

int main(){

	// Configurar a localização global para o programa
	setlocale(LC_ALL, "portuguese");

	Directories& dirs = Directories::getInstance();
	dirs.Initialize();

	//importação dos arquivos .json utilizados no programa
	//operator_auth (variavel onde sera armazenado) operator_auth_dir(diretorio do arquivo json)
	Directories::operator_auth = Directories::import_file(Directories::operator_auth_dir).second;
	Directories::championship_configs = Directories::import_file(Directories::championship_configs_dir).second;

	//coletar os dados de login do arquivo .json
	MySQL_Auth mysql_auth(MySQL_Server, MySQL_Username, MySQL_Password);

	//fazer a conexão no banco MySQL e gerar os objetos.
	MySQL::Connect(mysql_auth.getMySQLAuth()["server"], mysql_auth.getMySQLAuth()["username"], mysql_auth.getMySQLAuth()["password"]);

	Directories::registerAllDrivers();
	Exlog(AMARELO + __func__, VERDE + "All drivers registered!");

	//Iniciar o serviço do bot do discord
	thread Thread_01(Discord_Tools::discord_messenger);

	do { this_thread::sleep_for(chrono::seconds(2)); Exlog(AMARELO + __func__, VERDE + "Waiting for Discord Tools initialize..."); } while (Directories::DiscordToolsRoutesOnEnd == false);

	//iniciar o serviço Analysis Tools
	thread Thread_02(Analysis_Tools::operator_menu_main);

	do { this_thread::sleep_for(chrono::seconds(2)); Exlog(AMARELO + __func__, VERDE + "Waiting for AnalysisRoutes initialize..."); } while (Directories::AnalysisRoutesOnEnd == false);

	//Iniciar o serviço do Ranking Tools
	thread Thread_03(Ranking_Tools::operator_menu_main);

	Thread_01.join();
	Thread_02.join();
	Thread_03.join();
	
	Exlog(AMARELO + __func__, VERDE + "All done!");

	system("pause");

	return 0;

}