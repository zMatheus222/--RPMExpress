#include "default_functions.hpp"
#include "mysql_functions.hpp"
#include "RankingTools.hpp"

#include <regex>
#include <iostream>
#include <fstream>

#include <map>
#include <utility>

using namespace Ranking_Tools;

#define leaderBoardLines RankingJson["sessionResult"]["leaderBoardLines"]
#define raceNumber  RankingJson["sessionResult"]["leaderBoardLines"][i]["car"]["raceNumber"]
#define carModel    RankingJson["sessionResult"]["leaderBoardLines"][i]["car"]["carModel"]
#define cupCategory RankingJson["sessionResult"]["leaderBoardLines"][i]["car"]["cupCategory"]
#define teamName    RankingJson["sessionResult"]["leaderBoardLines"][i]["car"]["teamName"]
#define firstName   RankingJson["sessionResult"]["leaderBoardLines"][i]["currentDriver"]["firstName"]
#define lastName    RankingJson["sessionResult"]["leaderBoardLines"][i]["currentDriver"]["lastName"]
#define shortName   RankingJson["sessionResult"]["leaderBoardLines"][i]["currentDriver"]["shortName"]
#define playerId    RankingJson["sessionResult"]["leaderBoardLines"][i]["currentDriver"]["playerId"]
#define bestLap     RankingJson["sessionResult"]["leaderBoardLines"][i]["timing"]["bestLap"]
#define bestSector1 RankingJson["sessionResult"]["leaderBoardLines"][i]["timing"]["bestSplits"][0]
#define bestSector2 RankingJson["sessionResult"]["leaderBoardLines"][i]["timing"]["bestSplits"][1]
#define bestSector3 RankingJson["sessionResult"]["leaderBoardLines"][i]["timing"]["bestSplits"][2]
#define totalTime   RankingJson["sessionResult"]["leaderBoardLines"][i]["timing"]["totalTime"]
#define lapCount    RankingJson["sessionResult"]["leaderBoardLines"][i]["timing"]["lapCount"]


//#include <boost/locale/encoding_utf.hpp>

using namespace TXT_Style;
using namespace MySQL;
using namespace std;

struct compare {

	bool operator()(const pair<int, int>& a, const pair<int, int>& b) const {

		if (a.first == b.first) {

			if (a.second == b.second) {

				return false; // Os pares são iguais

			}

			return a.second < b.second; // Para o segundo int em ordem crescente
		}

		return a.first > b.first; // Para o primeiro int em ordem decrescente
	}

};

string remover_acentuacao(string* file_data) {

	Exlog(MAGENTA + __func__, StartMsg);

	string fixed_data;

	for (const char& caractere : *file_data) {

		char c;

		switch (caractere) {
		case 'á': case 'à': case 'ã': case 'â': case 'Á': case 'À': case 'Ã': case 'Â':
			c = 'a';
			break;
		case 'é': case 'è': case 'ê': case 'É': case 'È': case 'Ê':
			c = 'e';
			break;
		case 'í': case 'ì': case 'Í': case 'Ì':
			c = 'i';
			break;
		case 'ó': case 'ò': case 'õ': case 'ô': case 'Ó': case 'Ò': case 'Õ': case 'Ô':
			c = 'o';
			break;
		case 'ú': case 'ù': case 'û': case 'Ú': case 'Ù': case 'Û':
			c = 'u';
			break;
		case 'ç': case 'Ç':
			c = 'c';
			break;
		case '¾': case '¿': case '³': case '©': case '§': case '£':
			c = ' ';
			break;
		default:
			c = caractere;
		}

		fixed_data += c;

	}

	Exlog(MAGENTA + __func__, EndMsg);

	return fixed_data;
}

std::string convertUTF16LEtoUTF8(const std::wstring& utf16Content) {

	Exlog(AMARELO + __func__, StartMsg);

    std::string utf8Content;
    bool start_process = false;

    for (size_t i = 0; i < utf16Content.size(); ++i) {
        wchar_t wchar = utf16Content[i];

        if (wchar == '{') {
            start_process = true;
			utf8Content += wchar;
            continue;  // Pular o caractere '{' na saída UTF-8
        }

        if (start_process) {
            // Restante do código de conversão UTF-8 aqui

            // Ignorar caracteres nulos
            if (wchar == 0) {
                continue;
            }

            if (wchar <= 0x7F) {
                utf8Content += static_cast<char>(wchar);
            } else if (wchar <= 0x7FF) {
                utf8Content += static_cast<char>(0xC0 | (wchar >> 6));
                utf8Content += static_cast<char>(0x80 | (wchar & 0x3F));
            } else if (wchar <= 0xFFFF) {
                utf8Content += static_cast<char>(0xE0 | (wchar >> 12));
                utf8Content += static_cast<char>(0x80 | ((wchar >> 6) & 0x3F));
                utf8Content += static_cast<char>(0x80 | (wchar & 0x3F));
            } else if (wchar <= 0x10FFFF) {
                utf8Content += static_cast<char>(0xF0 | (wchar >> 18));
                utf8Content += static_cast<char>(0x80 | ((wchar >> 12) & 0x3F));
                utf8Content += static_cast<char>(0x80 | ((wchar >> 6) & 0x3F));
                utf8Content += static_cast<char>(0x80 | (wchar & 0x3F));
            }
        }
    }

	Exlog(AMARELO + __func__, VERDE + "wstring convertida para string com sucesso.");

	/*
    // Remover espaços e quebras de linha desnecessários
    utf8Content.erase(
    std::remove_if(utf8Content.begin(), utf8Content.end(), [](char c) {
        return std::isspace(static_cast<unsigned char>(c));
    }),
    utf8Content.end());
	
	Exlog(AMARELO + __func__, VERDE + "Espaços e quebras de linha removidos com sucesso.");
	*/

	Exlog(AMARELO + __func__, EndMsg);

    return utf8Content;
}

map<string, string> getDetailsByFile(string *file_name) {

	Exlog(AMARELO + __func__, StartMsg);

	Exlog(AMARELO + __func__, BRANCO + "file_name: " + CIANO + *file_name);

	map<string, string> DetailsByFile;

	smatch sma;

	if (regex_search(*file_name, sma, regex("(T[0-9]+)\\_(E[0-9]+)\\_([0-9]{2})([0-9]{2})([0-9]{2})\\_([0-9]{2})([0-9]{2})([0-9]{2})\\_R([0-9]+)?.+"))) {

		Exlog(AMARELO + __func__, BRANCO + "sma.size(): " + CIANO + to_string(sma.size()));

		DetailsByFile["Season"] = sma[1]; DetailsByFile["stage"] = sma[2];
		DetailsByFile["year"] =   sma[3]; DetailsByFile["month"] = sma[4];
		DetailsByFile["day"] =    sma[5]; DetailsByFile["hour"] = sma[6];
		DetailsByFile["minute"] = sma[7]; DetailsByFile["seconds"] = sma[8];

		//pegar a bateria, se for vazio é a bateria 1 (B1) se não é a bateria referente ao regex
		(sma[9].str().empty()) ? DetailsByFile["batery"] = "B1" : DetailsByFile["batery"] = "B" + sma[9].str();

		//definir StageBatery exemplo: 'E1B1'
		DetailsByFile["StageBatery"] = DetailsByFile["stage"] + DetailsByFile["batery"];

	}
	else {
		Exlog(AMARELO + __func__, VERMELHO + "Não foi possível encontrar a season do arquivo: " + *file_name);
		system("pause");
		exit(1);
	}

	Exlog(AMARELO + __func__, EndMsg);

	return DetailsByFile;

}

void update_backend_table(vector<string>& new_ranking_files) {

	Exlog(AMARELO + __func__, StartMsg);

	//fazer varredura em todos os arquivos .json da pasta
	//vector<string> ranking_files = Directories::list_files(Directories::results_dir.c_str(), true);

	Exlog(AMARELO + __func__, VERDE + "Arquivos recebidos: " + to_string(new_ranking_files.size()));

	//loop de verificação em cada arquivo encontrado
	
	for (const auto& current_file : new_ranking_files) {
		
		//Abrir arquivo e buscar no nome do servidor a temporada e etapa
		Exlog(AMARELO + __func__, "to_import_wfile: " + current_file);

		wstring json_full = Directories::import_wfile(Directories::results_dir.c_str() + string("/") + current_file);
		Exlog(AMARELO + __func__, VERDE + "Arquivo importado com sucesso. (wstring json_full ready)");
		
		string file_data = convertUTF16LEtoUTF8(json_full);
		Exlog(AMARELO + __func__, VERDE + "Arquivo convertido para string com sucesso. (string file_data ready)");

		remover_acentuacao(&file_data);
		
		json RankingJson = json::parse(file_data);
		Exlog(AMARELO + __func__, VERDE + "Arquivo convertido para json com sucesso (json RankingJson ready)");

		string fileName = current_file;
		const string serverName = RankingJson["serverName"];
		const string sessionType = RankingJson["sessionType"];
		const string trackName = RankingJson["trackName"];

		//fazer filtro pelo nome do servidor e descobrir a season
		map<string, string> DetailsByFile = getDetailsByFile(&fileName);

		//montar o nome da table
		string tableName = DetailsByFile["stage"] + "_" + trackName + "_" + sessionType;

		Exlog(VERDE + __func__, BRANCO + "to_search_in_rpm_base: " + CIANO + DetailsByFile["Season"] + BRANCO + " | " + CIANO + tableName + BRANCO + " | " + CIANO + QueryResultValues);

		//verificar se o schema atual existe
		Directories::search_in_rpm_base(DetailsByFile["Season"], tableName, QueryResultValues);

		if (!MySQL::verify_existing_content(DetailsByFile["Season"], tableName)) {

			Exlog(AMARELO + __func__, VERDE + "Dados não existentes. na table: " + CIANO + tableName);
		
			//export_file(&file_data, current_file);

			cout << "RankingJson.size(): " << leaderBoardLines.size() << "\n";

			//logica para classificar pelo total time e lap count
		 
			multimap<pair<int, int>, vector<string>, compare> ranking;
		
			for (int i = 0; i < leaderBoardLines.size(); i++) {
			
				vector<string> moreData;

				moreData.push_back(to_string(raceNumber.get<int>()));
				moreData.push_back(to_string(carModel.get<int>()));
				moreData.push_back(to_string(cupCategory.get<int>()));
				moreData.push_back(teamName.get<string>());
				moreData.push_back(firstName.get<string>());
				moreData.push_back(lastName.get<string>());
				moreData.push_back(shortName.get<string>());
				moreData.push_back(playerId.get<string>());
				moreData.push_back(to_string(bestSector1.get<int>()));
				moreData.push_back(to_string(bestSector2.get<int>()));
				moreData.push_back(to_string(bestSector3.get<int>()));
				moreData.push_back(to_string(bestLap.get<int>()));

				//se o valor não for null insira os pontos. //necessario pois podem ter por exemplo 82 pilotos, mas no score menos de 80 pontuações.
				int Pontos{ 0 };
				if (!Directories::championship_configs["Score"][i + 1].is_null()) {
					Pontos = Directories::championship_configs["Score"][i + 1];
				}

				//montar o multiplicador de pontos
				double Multiplier = Directories::championship_configs["ScoreMultiplier"][DetailsByFile["StageBatery"]]; Exlog(AMARELO + __func__, VERDE + "Multiplier: " + CIANO + to_string(Multiplier));

				moreData.push_back(to_string(Pontos * Multiplier));

				ranking.insert(make_pair(make_pair(lapCount, totalTime), moreData));

				moreData.clear();

			}

			for (auto it = ranking.begin(); it != ranking.end(); ++it) {

				map<string, string> tableData;

				tableData.insert(make_pair("fileName", fileName));
				tableData.insert(make_pair("serverName", serverName));
				tableData.insert(make_pair("sessionType", sessionType));
				tableData.insert(make_pair("trackName", trackName));

				tableData.insert(make_pair("Season", DetailsByFile["Season"]));
				tableData.insert(make_pair("tableName", tableName));

				tableData.insert(make_pair("raceNumber", it->second[0]));
				tableData.insert(make_pair("carModel", it->second[1]));
				tableData.insert(make_pair("cupCategory", it->second[2]));
				tableData.insert(make_pair("teamName", it->second[3]));
				tableData.insert(make_pair("firstName", it->second[4]));
				tableData.insert(make_pair("lastName", it->second[5]));
				tableData.insert(make_pair("shortName", it->second[6]));
				tableData.insert(make_pair("playerId", it->second[7]));
				tableData.insert(make_pair("bestSector1", it->second[8]));
				tableData.insert(make_pair("bestSector2", it->second[9]));
				tableData.insert(make_pair("bestSector3", it->second[10]));
				tableData.insert(make_pair("bestLap", it->second[11]));
				tableData.insert(make_pair("lapCount", to_string(it->first.first)));
				tableData.insert(make_pair("totalTime", to_string(it->first.second)));
				tableData.insert(make_pair("Pontos", it->second[12]));

				Exlog(VERDE + __func__, BRANCO + "size before insert_on_results_table: " + CIANO + to_string(tableData.size()));

				MySQL::insert_on_results_table(tableData);

				tableData.clear();

			}

			Exlog(AMARELO + __func__, VERDE + "Dados inseridos com sucesso na table: " + CIANO + tableName);
		}
		else {
			Exlog(AMARELO + __func__, AMARELO + "Dados já existentes. na table: " + CIANO + tableName);
		}	

	}

	Exlog(AMARELO + __func__, EndMsg);

}

void update_frontend_table() {

	try {

		vector<string> schemas = list_schemas(regex("T\\d{1,3}"));

		//primeiro importamos um pedaço do html + o css completo

		ifstream show_html(Directories::table_dir);

		//começa salvar todas as linhas do arquivo importado na var, table_html
		string table_html{ "" };
		string line{ "" };
		bool STOP = false;
		while (getline(show_html, line, '\n')) {
			
			//quando chegar na parte do codigo com STOP1 pausa a importação
			if (regex_search(line, regex(".+START[0-9]+.+"))) {
				Exlog(AMARELO + __func__, VERDE + line);
				//pegamos as informações do banco de dados para criar um codigo html
				table_html += MySQL::results_html_fill(schemas, line.c_str());
				STOP = true;
			}
			//quando chegar na parte do codigo com STOP2 volta a permitir a importação
			else if (regex_search(line, regex("--STOP--"))) {
				Exlog(AMARELO + __func__, VERDE + "<!--STOP-->");
				STOP = false;
			}
			//salva as linhas do html quando permitido
			if (STOP == false) {
				table_html += line + "\n";
			}
		}

		show_html.close();

		std::ofstream html_out(Directories::table_after_dir);

		html_out << table_html;
		
		//Exlog(AMARELO + __func__, "Arquivo html: " + table_html);

		Exlog(AMARELO + __func__, VERDE + "Arquivo html atualizado com sucesso.");

		html_out.close();
	}
	catch (const std::exception& e) {
		// Captura exceções do tipo std::exception (ou suas derivadas)
		Exlog(AMARELO + __func__, VERMELHO + "Erro durante a chamada de update_show[2args]: " + e.what());
	}

}

void rankingFileDetector() { Exlog(AMARELO + __func__, StartMsg);

	int resultFileCount = 0;
	int lastRankingFileCount = 0;

	do {
		//fazer varredura em todos os arquivos .json da pasta
		vector<string> ranking_files = Directories::list_files(Directories::results_dir.c_str(), true);

		Exlog(AMARELO + __func__, VERDE + "Arquivos encontrados: " + to_string(ranking_files.size()));

		//loop de verificação em cada arquivo encontrado

		int resultFileCount = 0;
		vector<string> new_ranking_files;
		
		for (const auto& current_file : ranking_files) { Exlog(AMARELO + __func__, "to_import_wfile: " + current_file);

			if (regex_search(current_file, regex("T[0-9]+_E[0-9]+_\\w+_R[0-9]?.json"))) {

				resultFileCount++;

				if (resultFileCount > lastRankingFileCount) {
					Exlog(AMARELO + __func__, VERDE + "Inserindo novo arquivo de resultado ao vector.");
					new_ranking_files.push_back(current_file);
				}
			}

		}

		Exlog(AMARELO + __func__, VERDE + "Novos arquivos de resultado encontrados:");
		for (auto& rf : new_ranking_files) {
			Exlog(AMARELO + __func__, BRANCO + "-> ranking_files: " + rf);
		}

		Exlog(AMARELO + __func__, VERDE + "[resultFileCount]: " + to_string(resultFileCount)); Exlog(AMARELO + __func__, VERDE + "[lastRankingFileCount]: " + to_string(lastRankingFileCount));

		if (resultFileCount > lastRankingFileCount) {

			Exlog(AMARELO + __func__, VERDE + "Novo arquivo de resultado encontrado.");

			update_backend_table(new_ranking_files);
			update_frontend_table();
			update_geral_ranking();

		}

		lastRankingFileCount = resultFileCount;

		Exlog(AMARELO + __func__, VERDE + "[resultFileCount]: " + to_string(resultFileCount));
		Exlog(AMARELO + __func__, VERDE + "[lastRankingFileCount]: " + to_string(lastRankingFileCount));
		Exlog(AMARELO + __func__, VERDE + "Aguardando 15 segundos para nova verificação.");

		this_thread::sleep_for(chrono::seconds(15));

	} while (true);
}

int Ranking_Tools::operator_menu_main() {

	rankingFileDetector();

	exit(22);

	//update_backend_table();

	//update_frontend_table();

		

	// iniciar serviço de coleta de resultados, onde teremos um detector de arquivos .json
	// 
	// 
	// 
	// detector de horário, onde teremos um arquivo de configuração com os horários de cada etapa
	// quando a etapa terminar, buscar por novos resultados na pasta e subir para o MySQL
		// definir um padrão para o nome do servidor na configuração para
		// [T21-E5] RPM Esports - OFICIAL (Linktr.ee/RPMEsports)
		// 
	return 0;
}