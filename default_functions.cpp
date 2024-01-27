#include "default_functions.hpp"
#include "mysql_functions.hpp"

#include <iostream>
#include <fstream>
#include <regex>

#include <windows.h>

using namespace TXT_Style;
using namespace std;

// Inicialização das variáveis estáticas de Diretories.
string Directories::base_dir; string Directories::templates_dir; string Directories::static_dir; string Directories::const_dir; string Directories::results_dir; string Directories::operator_auth_dir; string Directories::championship_configs_dir; string Directories::analysis_request_dir; string Directories::analysis_request_after_dir; string Directories::analysis_show_dir; string Directories::analysis_show_after_dir; string Directories::operator_menu_dir; string Directories::table_dir; string Directories::table_after_dir; string Directories::log_files_dir; json Directories::operator_auth; json Directories::championship_configs;

mutex df_mutex; 

//map<string, vector<string>> Directories::rpm_base;

bool Directories::AnalysisRoutesOnEnd = false;
bool Directories::RankingToolsOnEnd = false;
bool Directories::DiscordToolsRoutesOnEnd = false;
bool Directories::RPMDownloaderOnEnd = false;

//bool Directories::RPMDownloaderOnEnd = false;

void Directories::Initialize() {

	Exlog("INFO", AMARELO + __FUNCTION__, StartMsg);

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	// Obter o caminho do diretório do executável
	filesystem::path exePath(buffer);

	string exedir = exePath.parent_path().string();

	// Usar o caminho do executável para construir caminhos para outros arquivos
	operator_auth_dir = exedir + "/operator_auth.json";
	championship_configs_dir = exedir + "/templates/championship_configs.json";

	base_dir = exedir;

	static_dir = exedir + "/templates/static";

	templates_dir = exedir + "/templates";

	analysis_request_dir = exedir + "/templates/analysis_request.html";
	analysis_request_after_dir = exedir + "/templates/analysis_request_after.html";
	analysis_show_dir = exedir + "/templates/analysis_show.html";
	analysis_show_after_dir = exedir + "/templates/analysis_show_after.html";
	operator_menu_dir = exedir + "/templates/op_menu.html";
	table_dir = exedir + "/templates/tables.html";
	table_after_dir = exedir + "/templates/table_after.html";
	results_dir = exedir + "/results";

	log_files_dir = exedir + "/logs";

	// Exibe o caminho do diretório
	Exlog("INFO", AMARELO + __FUNCTION__, "base_dir: " + base_dir);

	Exlog("INFO", AMARELO + __FUNCTION__, EndMsg);

}

vector<string> Directories::list_files(const char* path, bool ignore_folders) {

	//retorna a lista de arquivos de algum diretório.

	vector<string> files;

	filesystem::path diretorio(path);

	if (filesystem::is_directory(diretorio)) {

		filesystem::directory_iterator it(diretorio);
		filesystem::directory_iterator end;

		while (it != end) {
			if (!ignore_folders || !filesystem::is_directory(it->path())) {
				files.push_back(it->path().filename().string());
			}
			++it;
		}
	}
	else {
		Exlog("ERROR", AMARELO + __func__, VERMELHO + "O caminho fornecido nao e um diretorio valido.");
	}

	//Exlog(AMARELO + __func__, EndMsg);

	return files;

}

pair<string,json> Directories::import_file(const string& file_path) {

	lock_guard<mutex> lock(df_mutex);

	Exlog("INFO", AMARELO + __func__, StartMsg);

	int buffer_size;
	string string_export;

	try {
		//variaveis para exportação
		json json_file = nullptr;
		string file_type{ "" };
		
		//detectar o tipo de arquivo
		smatch sma;
		if (regex_search(file_path, sma, regex(".+\\.(.+)"))) {

			Exlog("INFO", AMARELO + __func__, "Arquivo do tipo: " + string(sma[1]) + " | " + file_path + " importado som sucesso.");
			file_type = sma[1];
		}
		else {

			Exlog("ERROR", AMARELO + __func__, VERMELHO + "Nao foi possivel identificar o tipo do arquivo: " + file_path);
		}

		ifstream imp_file(file_path);

		if (!imp_file.is_open()) {
			Exlog("ERROR", AMARELO + __func__, VERMELHO + "Falha ao importar o arquivo. file_path: " + file_path);
		}

		//se o arquivo for json fazer o parse.
		if (file_type == "json") {

			try {
				// Usar um istringstream para fazer o parse do JSON
				imp_file >> json_file;
			}
			catch (const json::parse_error& e) {
				Exlog("ERROR", AMARELO + __func__, VERMELHO + "Erro ao tentar fazer o Parse do JSON: " + e.what());
			}
		}
		else {
			string line;
			while (getline(imp_file, line)) {
				string_export += line + "\n";
			}
			buffer_size = string_export.size();
		}

		Exlog("INFO", AMARELO + __func__, "Criando par de dados para retorno.");

		//criar um par para o retorno, um deles será vazio, o outro terá o conteudo (dependendo o que for passado na função)
		pair<string, json> data = make_pair(string_export, json_file);

		imp_file.close();

		Exlog("INFO", AMARELO + __func__, EndMsg);

		return data;
	}
	catch (const exception& e) {

		Exlog("ERROR", AMARELO + __func__, VERMELHO + "Erro durante a chamada (catch): " + e.what());
		// Trate a exceção ou adicione mais informações ao log

		// Mostrar o conteúdo e o tamanho da variável 'buffer'
		Exlog("ERROR", AMARELO + __func__, "Conteudo da variavel 'buffer': " + string_export);
		Exlog("ERROR", AMARELO + __func__, "Tamanho da variável 'buffer': " + to_string(buffer_size) + " bytes");
	}
}

wstring Directories::import_wfile(const string& file_path) {

	Exlog("INFO", AMARELO + __func__, StartMsg);

	int buffer_size;
	wstring string_export;

	try {
		//variaveis para exportação
		string file_type{ "" };

		//detectar o tipo de arquivo
		smatch sma;
		if (regex_search(file_path, sma, regex(".+\\.(.+)"))) {

			Exlog("INFO", AMARELO + __func__, "Arquivo do tipo: " + string(sma[1]) + " | " + file_path + " importado som sucesso.");
			file_type = sma[1];
		}
		else {

			Exlog("INFO", AMARELO + __func__, VERMELHO + "Nao foi possivel identificar o tipo do arquivo: " + file_path);
		}

		wifstream imp_file(file_path);

		if (!imp_file.is_open()) {
			Exlog("INFO", AMARELO + __func__, VERMELHO + "Falha ao importar o arquivo. file_path: " + file_path);
		}

		wstring line;
		while (getline(imp_file, line)) {
			string_export += line + L"\n";
		}

		buffer_size = string_export.size();

		imp_file.close();

		Exlog("INFO", AMARELO + __func__, EndMsg);

		return string_export;
	}
	catch (const exception& e) {

		Exlog("ERROR", AMARELO + __func__, VERMELHO + "Erro durante a chamada (catch): " + e.what());
		// Trate a exceção ou adicione mais informações ao log

		// Mostrar o conteúdo e o tamanho da variável 'buffer'
		Exlog("ERROR", AMARELO + __func__, "Tamanho da variável 'buffer': " + to_string(buffer_size) + " bytes");
	}
}

/*
void export_file(string* file_data, const string& file_name) {

	Exlog("INFO", AMARELO + __func__, StartMsg);

	ofstream exp_file(Directories::results_dir + "/converted/teste.json");
	Exlog("INFO", AMARELO + __func__, VERDE + "Arquivo corrigido e exportado para: " + Directories::results_dir + "/converted/teste.json");

	exp_file << *file_data;

	Exlog("INFO", AMARELO + __func__, EndMsg);

}
*/

map<string, string> Directories::search_in_rpm_base(string needed_schema, string needed_table, string table_query) {

	//lock_guard<mutex> lock(mysql_mutex);

	Exlog("INFO", AMARELO + __func__, StartMsg);

	Exlog("INFO", AMARELO + __func__, "Received values:");
	Exlog("INFO", AMARELO + __func__, "needed_schema: " + needed_schema);
	Exlog("INFO", AMARELO + __func__, "needed_table: " + needed_table);
	Exlog("INFO", AMARELO + __func__, "table_query: " + table_query);

	map<string, string> SchemaTable;
	bool founded_schema = false;
	bool founded_table = false;

	Exlog("INFO", AMARELO + __func__, "needed_schema: " + needed_schema);

	//se a string de needed_schema que foi passada não for "none", realizar os procedimentos de schema.
	if (needed_schema != "none") {

		int attempts = 0;
		do {

			//procurar na lista de schemas o que a gente precisa
			vector<string> current_schema = MySQL::list_schemas(regex(needed_schema));

			Exlog("INFO", AMARELO + __func__, "current_schema[" + to_string(current_schema.size() - 1) + "]: " + current_schema[current_schema.size() - 1]);

			//detectar se encontramos ou não o schema
			((current_schema[current_schema.size() - 1] != "none") && (current_schema.size() < 2)) ? founded_schema = true : founded_schema = false;

			Exlog("INFO", AMARELO + __func__, "founded_schema: " + to_string(founded_schema));

			//se não encontramos o schema, criar ele e adicionar o numero de tentativas.
			
			if (founded_schema) {
				Exlog("INFO", AMARELO + __func__,  VERDE + "Schema " + needed_schema + " encontrado, salvando no map...");
				SchemaTable["Schema"] = current_schema[current_schema.size() - 1];
			}
			else {

				Exlog("INFO", AMARELO + __func__, AMARELO + "Schema " + needed_schema + " nao encontrado, Chamando funcao create_schema...");
				
				MySQL::create_schema(needed_schema);
				founded_schema = true;

				attempts++;

				if (attempts >= 5) {
					Exlog("ERROR", AMARELO + __func__, VERMELHO + "Nao foi possivel criar o schema, erro critico.");
					this_thread::sleep_for(chrono::seconds(1));
					break;
				}
			}

		} while (!founded_schema);

	}

	Exlog("INFO", AMARELO + __func__, "needed_table: " + needed_table);

	if (needed_table != "none") {

		int attempts = 0;
		do {

			//procurar na lista de tables o que a gente precisa
			vector<string> current_table = MySQL::list_tables(needed_schema, regex(needed_table));

			Exlog("INFO", AMARELO + __func__, "current_table[" + to_string(current_table.size() - 1) + "]: " + current_table[current_table.size() - 1]);
			          
			//detectar se encontramos ou não a table
			(regex_search(current_table[current_table.size() -1], regex(needed_table))) ? founded_table = true : founded_table = false;
			Exlog("INFO", AMARELO + __func__, "founded_table: " + to_string(founded_table));

			//se não encontramos a table, criar ele e adicionar o numero de tentativas.|
			if (founded_table) {
				Exlog("INFO", AMARELO + __func__, VERDE + "Table " + needed_table + " encontrado, salvando no map...");
				SchemaTable["Table"] = current_table[current_table.size() - 1];
			}
			else {

				Exlog("INFO", AMARELO + __func__, AMARELO + "Table " + needed_table + " nao encontrado, Chamando funcao create_table...");

				MySQL::create_table(needed_schema, needed_table, false, table_query);
				founded_table = true;
				
				attempts++;

				if (attempts >= 5) {
					Exlog("ERROR", AMARELO + __func__, VERMELHO + "Nao foi possivel criar o table, erro critico.");
					this_thread::sleep_for(chrono::seconds(1));
					break;
				}
			}

		} while (!founded_table);

	}

	return SchemaTable;

}

string validate_link(string& link) {

	Exlog("INFO", AMARELO + __func__, StartMsg);

	string received_url = link;

	if (regex_search(received_url, regex("([A-Za-z0-9]+\\:\\/\\/)(\\w+.){3}\\/.+"))) {
		Exlog("INFO", AMARELO + __func__, "Received a valid url");
		return received_url;
	}

	smatch capgroup;

	if (regex_search(received_url, capgroup, regex("(www.)?((?:\\w+.)+)"))) {

		Exlog("INFO", AMARELO + __func__, "Received a invalid url: " + received_url);

		if (capgroup[1] == "www.") {
			received_url = "https://" + received_url;
			return received_url;
		}
		else {
			received_url = "https://www." + received_url;
			return received_url;
		}

	}

	Exlog("INFO", AMARELO + __func__, EndMsg);

	return received_url;

}

void ExportLogs(const string& logn, string& log_data) {

	string export_dir = Directories::log_files_dir + "/exlog_" + logn + ".log";

	ofstream log_file(export_dir, ios::app);

	if (!log_file.is_open()) {
		Exlog("ERROR", VERMELHO + __func__, "***Falha ao exportar o arquivo de log. log_type: " + logn + "***\n");
	}

	log_file << log_data;

	log_file.close();

}

void Exlog(string log_type, string function, string message) {

	if (Debug_Logs == "true") {

		string formatted_color_msg = NEGRITO + BRANCO + "[" + log_type + "][" + function + BRANCO + "] " + PADRAO + message + ENDS;
		string simple_msg = "[" + log_type + "][" + function + "] " + message + "\n";

		if ((regex_search(function, regex(Exlog_Shows))) || (regex_match(string(Exlog_Shows), regex("All")))) {
			(Exlog_Colors == "true") ? cout << formatted_color_msg : cout << simple_msg;
		}

		if (File_Logs == "true") ExportLogs("27_01_2024", simple_msg);

	}
}

/*
void Exlog(string log_type, const char* function, const char* service, const char* route_type, const char* message) {

	if (Debug_Logs == "true") {

		string formatted_color_msg = NEGRITO + BRANCO + "[" + PRETO + function + PADRAO + BRANCO + "::" + MAGENTA + service + BRANCO + "::" + CIANO + route_type + NEGRITO + BRANCO + "] " + PADRAO + BRANCO + message + ENDS;
		string simple_msg = "[" + string(function) + "::" + service + "::" + route_type + "] " + string(message) + "\n";

		if ((regex_search(function, regex(Exlog_Shows))) || (regex_match(string(Exlog_Shows), regex("All")))) {
			(Exlog_Colors == "true") ? cout << formatted_color_msg : cout << simple_msg;
		}

		if (File_Logs == "true") ExportLogs("27_01_2024", simple_msg);
	}
}


void Exlog(string log_type, const char* function, const char* service, const char* route_type, string message) {

	if (Debug_Logs == "true") {

		if ((regex_search(function, regex(Exlog_Shows))) || (regex_match(string(Exlog_Shows), regex("All")))) {
			
			if (Exlog_Colors == "true") {
				cout << NEGRITO << BRANCO << "[" << PRETO << function << PADRAO << BRANCO << "::" << MAGENTA << service << BRANCO << "::" << CIANO << route_type << NEGRITO << BRANCO << "] " << PADRAO << BRANCO << message << ENDS;
			}
			else {
				cout << "[" << function << "::" << service << "::" << route_type << "] " << message << endl;
			}

		}
	}	
}
*/