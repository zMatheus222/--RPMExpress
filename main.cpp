#include "default_functions.hpp"
#include "mysql_functions.hpp"
#include "AnalysisTools.hpp"
#include "DiscordBot.hpp"
#include "RankingTools.hpp"

#include <iostream>

using namespace TXT_Style;
using namespace std;

void waitf(int max_attemps, bool& function) {

	int loopc = 1;

	do {

		this_thread::sleep_for(chrono::seconds(2));
		Exlog(AMARELO + __func__, VERDE + "Waiting for function initialize... attempt: " + to_string(loopc) + "/" + to_string(max_attemps));
		loopc++;
		if (loopc > max_attemps) break;

	} while (function == false);

}

int main(){

	// Configurar a localiza��o global para o programa
	setlocale(LC_ALL, "portuguese");

	Directories& dirs = Directories::getInstance();
	dirs.Initialize();

	//importa��o dos arquivos .json utilizados no programa
	//operator_auth (variavel onde sera armazenado) operator_auth_dir(diretorio do arquivo json)
	Directories::operator_auth = Directories::import_file(Directories::operator_auth_dir).second;
	Directories::championship_configs = Directories::import_file(Directories::championship_configs_dir).second;

	//coletar os dados de login do arquivo .json
	MySQL_Auth mysql_auth(MySQL_Server, MySQL_Username, MySQL_Password);

	//fazer a conex�o no banco MySQL e gerar os objetos.
	MySQL::Connect(mysql_auth.getMySQLAuth()["server"], mysql_auth.getMySQLAuth()["username"], mysql_auth.getMySQLAuth()["password"]);

	Directories::registerAllDrivers();
	Exlog(AMARELO + __func__, VERDE + "All drivers registered!");

	//Iniciar o servi�o do bot do discord
	thread Thread_01(Discord_Tools::discord_messenger);

	waitf(10, Directories::DiscordToolsRoutesOnEnd);

	//iniciar o servi�o Analysis Tools
	thread Thread_02(Analysis_Tools::operator_menu_main);

	waitf(10, Directories::AnalysisRoutesOnEnd);

	//Iniciar o servi�o do Ranking Tools
	thread Thread_03(Ranking_Tools::operator_menu_main);

	waitf(10, Directories::RankingToolsOnEnd);

	//Iniciar o endpoint do RPM-Downloader
	thread Thread_04(rpm_downloader);

	waitf(10, Directories::RPMDownloaderOnEnd);

	cout << "\n\n";
	Exlog(AMARELO + __func__, MAGENTA + "All done! RPM Express Running!\n\n");

	Thread_01.join(); Thread_02.join();
	Thread_03.join(); Thread_04.join();

	cout << "\n\n";

	Exlog(AMARELO + __func__, VERMELHO + "RPM Express Finalizado!\n\n");

	system("pause");

	return 0;

}