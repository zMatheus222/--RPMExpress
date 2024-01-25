#include "AnalysisTools.hpp"

#include <fstream>
#include <regex>
#include <iostream>

#include <chrono>
#include <thread>
#include <array>

using namespace TXT_Style;
using namespace MySQL;
using namespace std;

map<string, string> Analysis_Tools::PickPunishmentBySituation(string& piloto_punido, string& situacaoPrincipal, string& situacaoIntrinseca, bool reincidencia) {

	map<string, string> map_punishments;

	//conversores de int para string caso necessário.

	map_punishments["sit-principal"] = Directories::championship_configs["PunishmentOptions"]["SituacoesPrincipais"][stoi(situacaoPrincipal) - 1];           //pega a situação principal com base no numero passsado no menu
	map_punishments["sit-intrinseca"] = Directories::championship_configs["PunishmentOptions"]["SituacoesIntrinsecas"][stoi(situacaoIntrinseca) - 1];        //pega a situação intrinseca com base no numero passado no menu
	map_punishments["SIPunicoes"] = Directories::championship_configs["PunishmentOptions"]["SIPunicoes"][stoi(situacaoIntrinseca) - 1];                      //pega a punição com base na situação intrinseca passada no menu
	map_punishments["reincidencia"] = to_string(reincidencia);                                                                                                          //pega a reincidencia com base no numero passado no menu                                                                                            //pega o id da analise

	smatch sma;

	//PUNIÇÕES POR TEMPO - pega o valor e dobra em caso de reincidência
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("[-+](\\d+)\\sSegundos$"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao por tempo.");
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[1]: " + CIANO + sma[1].str());

		int puni = stoi(sma[1].str());

		//aplicar dobro do valor caso seja reincidencia 
		(map_punishments["reincidencia"] == "true") ? puni *= 2 : puni *= 1;
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "ByTime::Detectada Reincidencia, Punição por tempo dobrada.");
		map_punishments["Punicao_Pontos"] = "0";
		map_punishments["Punicao_Tempo"] = to_string(puni);
		map_punishments["Punicoes_Adicionais"] = "Nenhuma";
	}
	//PUNIÇÕES POR PONTOS - pega o valor e dobra em caso de reincidência
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("[-+](\\d+)\\sPontos$"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao por pontos.");
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[1]: " + CIANO + sma[1].str());

		int puni = stoi(sma[1].str());

		//aplicar dobro do valor caso seja reincidencia 
		(map_punishments["reincidencia"] == "true") ? puni *= 2 : puni *= 1;
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + " ByPoints::Detectada Reincidencia, Punição por pontos dobrada.");

		map_punishments["Punicao_Pontos"] = to_string(puni);
		map_punishments["Punicao_Tempo"] = "0";
		map_punishments["Punicoes_Adicionais"] = "Nenhuma";
	}
	//PUNIÇÕES POR TEMPO E PONTOS
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("[-+](\\d+)\\sPontos\\s\\&\\s[-+](\\d+)\\sSegundos(?:\\s\\&\\s(\\w+))?$"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao por tempo e pontos.");
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[1]: " + CIANO + sma[1].str());
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[2]: " + CIANO + sma[2].str());

		int puni_p = stoi(sma[1].str());
		int puni_t = stoi(sma[2].str());
		//string puni_a{ "" };

		//detectar se há valor em sma[3] (punição adicional como DSQ, QB etc.)
		(sma[3].str().empty()) ? map_punishments["Punicoes_Adicionais"] = "Nenhuma" : map_punishments["Punicoes_Adicionais"] = sma[3].str();

		//aplicar dobro do valor caso seja reincidencia 
		(map_punishments["reincidencia"] == "true") ? puni_p *= 2, puni_t *= 2 : puni_p *= 1, puni_t *= 1;

		map_punishments["Punicao_Tempo"] = to_string(puni_t);
		map_punishments["Punicao_Pontos"] = to_string(puni_p);

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada Reincidencia, Punição por tempo e pontos dobrada. Punicao adicional: " + map_punishments["Punicoes_Adicionais"]);

	}

	//SEM PUNIÇÃO
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("Sem Puni..o"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao sem punição.");

		map_punishments["Punicao_Tempo"] = "0";
		map_punishments["Punicao_Pontos"] = "0";
		map_punishments["Punicoes_Adicionais"] = "Nenhuma";
	}

	//PUNIÇÕES POR PONTOS E BANIMENTO - pega o valor e dobra em caso de reincidência
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("[-+](\\d+)\\sPontos\\,\\s(\\w{2,4})\\s\\&\\s(?:(?:\\w+)\\s?)+"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao por pontos e banimento (si: 11)");
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[1]: " + CIANO + sma[1].str());
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[2]: " + CIANO + sma[2].str());

		int puni = stoi(sma[1].str());

		//detectar se há valor em sma[3] (punição adicional como DSQ, QB etc.)
		(sma[3].str().empty()) ? map_punishments["Punicoes_Adicionais"] = "Nenhuma" : map_punishments["Punicoes_Adicionais"] = sma[2].str() + " Banimento da proxima etapa.";

		//aplicar dobro do valor caso seja reincidencia 
		(map_punishments["reincidencia"] == "true") ? puni *= 2 : puni *= 1;
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "ByTime::Detectada Reincidencia, Punição por pontos dobrada.");
		map_punishments["Punicao_Pontos"] = to_string(puni);
		map_punishments["Punicao_Tempo"] = "0";

	}

	return map_punishments;

}

map<string,string> Analysis_Tools::PickPunishmentBySituation(json& data) {

	//string piloto_acusado{ "none" };
	string situacaoPrincipal{ "none" };
	string situacaoIntrinseca{ "none" };
	string reincidencia{ "none" };
	string AnalysisId{ "none" };
	//string etapa{ "none" };
	//string bateria{ "none" };

	//string reincidencia_discord{ "none" };

	map<string, string> map_punishments;

	//conversores de int para string caso necessário.

	(data["p_acusado"].is_string()) ? map_punishments["piloto_acusado"] = data["p_acusado"].get<string>() : map_punishments["piloto_acusado"] = to_string(data["p_acusado"].get<int>());

	(data["situacaoPrincipal"].is_string()) ? situacaoPrincipal = data["situacaoPrincipal"].get<string>() : situacaoPrincipal = to_string(data["situacaoPrincipal"].get<int>());
	(data["situacaoIntrinseca"].is_string()) ? situacaoIntrinseca = data["situacaoIntrinseca"].get<string>() : situacaoIntrinseca = to_string(data["situacaoIntrinseca"].get<int>());
	(data["reincidencia"].is_string()) ? reincidencia = data["reincidencia"].get<string>() : reincidencia = to_string(data["reincidencia"].get<bool>());
	(data["AnalysisId"].is_string()) ? AnalysisId = data["AnalysisId"].get<string>() : AnalysisId = to_string(data["AnalysisId"].get<int>());

	(data["etapa"].is_string()) ? map_punishments["etapa"] = data["etapa"].get<string>() : map_punishments["etapa"] = to_string(data["etapa"].get<int>());
	(data["bateria"].is_string()) ? map_punishments["bateria"] = data["bateria"].get<string>() : map_punishments["bateria"] = to_string(data["bateria"].get<int>());

	//salvar os dados no map_punishments
	map_punishments["Season"] = data["Season"].get<string>();                                                                                                //pega a season com base no numero passado no menu
	map_punishments["AnalysisTable"] = AnalysisDefaultTable.get<string>();                                                                                   //nome da table de analises
	map_punishments["sit-principal"] = Directories::championship_configs["PunishmentOptions"]["SituacoesPrincipais"][stoi(situacaoPrincipal) - 1];           //pega a situação principal com base no numero passsado no menu
	map_punishments["sit-intrinseca"] = Directories::championship_configs["PunishmentOptions"]["SituacoesIntrinsecas"][stoi(situacaoIntrinseca) - 1];        //pega a situação intrinseca com base no numero passado no menu
	map_punishments["SIPunicoes"] = Directories::championship_configs["PunishmentOptions"]["SIPunicoes"][stoi(situacaoIntrinseca) - 1];                      //pega a punição com base na situação intrinseca passada no menu
	map_punishments["reincidencia"] = reincidencia;                                                                                                          //pega a reincidencia com base no numero passado no menu
	map_punishments["AnalysisId"] = AnalysisId;                                                                                                              //pega o id da analise

	smatch sma;

	Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "punishment[3] (punicao a ser aplicada): " + CIANO + map_punishments["SIPunicoes"]);

	//PUNIÇÕES POR TEMPO - pega o valor e dobra em caso de reincidência
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("[-+](\\d+)\\sSegundos$"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao por tempo.");
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[1]: " + CIANO + sma[1].str());

		int puni = stoi(sma[1].str());

		//aplicar dobro do valor caso seja reincidencia 
		(map_punishments["reincidencia"] == "true") ? puni *= 2 : puni *= 1;
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "ByTime::Detectada Reincidencia, Punição por tempo dobrada.");
		map_punishments["Punicao_Pontos"] = "0";
		map_punishments["Punicao_Tempo"] = to_string(puni);
		map_punishments["Punicoes_Adicionais"] = "Nenhuma";
	}
	//PUNIÇÕES POR PONTOS - pega o valor e dobra em caso de reincidência
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("[-+](\\d+)\\sPontos$"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao por pontos.");
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[1]: " + CIANO + sma[1].str());

		int puni = stoi(sma[1].str());

		//aplicar dobro do valor caso seja reincidencia 
		(map_punishments["reincidencia"] == "true") ? puni *= 2 : puni *= 1;
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + " ByPoints::Detectada Reincidencia, Punição por pontos dobrada.");

		map_punishments["Punicao_Pontos"] = to_string(puni);
		map_punishments["Punicao_Tempo"] = "0";
		map_punishments["Punicoes_Adicionais"] = "Nenhuma";
	}
	//PUNIÇÕES POR TEMPO E PONTOS
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("[-+](\\d+)\\sPontos\\s\\&\\s[-+](\\d+)\\sSegundos(?:\\s\\&\\s(\\w+))?$"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao por tempo e pontos.");
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[1]: " + CIANO + sma[1].str());
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[2]: " + CIANO + sma[2].str());

		int puni_p = stoi(sma[1].str());
		int puni_t = stoi(sma[2].str());
		//string puni_a{ "" };

		//detectar se há valor em sma[3] (punição adicional como DSQ, QB etc.)
		(sma[3].str().empty()) ? map_punishments["Punicoes_Adicionais"] = "Nenhuma" : map_punishments["Punicoes_Adicionais"] = sma[3].str();

		//aplicar dobro do valor caso seja reincidencia 
		(map_punishments["reincidencia"] == "true") ? puni_p *= 2, puni_t *= 2 : puni_p *= 1, puni_t *= 1;

		map_punishments["Punicao_Tempo"] = to_string(puni_t);
		map_punishments["Punicao_Pontos"] = to_string(puni_p);

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada Reincidencia, Punição por tempo e pontos dobrada. Punicao adicional: " + map_punishments["Punicoes_Adicionais"]);

	}

	//SEM PUNIÇÃO
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("Sem Puni..o"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao sem punição.");

		map_punishments["Punicao_Tempo"] = "0";
		map_punishments["Punicao_Pontos"] = "0";
		map_punishments["Punicoes_Adicionais"] = "Nenhuma";
	}

	//PUNIÇÕES POR PONTOS E BANIMENTO - pega o valor e dobra em caso de reincidência
	if (regex_search(map_punishments["SIPunicoes"], sma, regex("[-+](\\d+)\\sPontos\\,\\s(\\w{2,4})\\s\\&\\s(?:(?:\\w+)\\s?)+"))) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "Detectada punicao por pontos e banimento (si: 11)");
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[1]: " + CIANO + sma[1].str());
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "sma[2]: " + CIANO + sma[2].str());

		int puni = stoi(sma[1].str());

		//detectar se há valor em sma[3] (punição adicional como DSQ, QB etc.)
		(sma[3].str().empty()) ? map_punishments["Punicoes_Adicionais"] = "Nenhuma" : map_punishments["Punicoes_Adicionais"] = sma[2].str() + " Banimento da proxima etapa.";

		//aplicar dobro do valor caso seja reincidencia 
		(map_punishments["reincidencia"] == "true") ? puni *= 2 : puni *= 1;
		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "ByTime::Detectada Reincidencia, Punição por pontos dobrada.");
		map_punishments["Punicao_Pontos"] = to_string(puni);
		map_punishments["Punicao_Tempo"] = "0";

	}

	Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", VERDE + "Lambda on /analysis_update - Ended");

	return map_punishments;

}

void Analysis_Tools::update_analysisRequest_show() {
	
	Exlog(AMARELO + __func__, StartMsg);

	try {

		//primeiro importamos um pedaço do html + o css completo
		ifstream show_html(Directories::analysis_request_dir);

		if (!show_html.is_open()) {
			Exlog(AMARELO + __func__, VERMELHO + "Nao foi possivel importar o arquivo html no fstream: show_html: " + Directories::analysis_show_dir);
		}

		//começa salvar todas as linhas do arquivo importado na var, table_html
		string table_html{ "" };
		string line{ "" };
		bool STOP = false;
		while (getline(show_html, line, '\n')) {

			//quando chegar na parte do codigo com STOP1 pausa a importação

			if (regex_search(line, regex(".+START[0-9]+.+"))) {
				//pegamos as informações do banco de dados para criar um codigo html
				Exlog(AMARELO + __func__, AMARELO + "Calling analysisRequest_html_fill | line.c_str: " + line.c_str());
				table_html += MySQL::analysisRequest_html_fill(line.c_str());
				STOP = true;
			}
			//quando chegar na parte do codigo com STOP2 volta a permitir a importação
			else if (regex_search(line, regex("--STOP--"))) {
				STOP = false;
			}
			//salva as linhas do html quando permitido
			if (STOP == false) {
				table_html += line + "\n";
			}
		}

		show_html.close();

		ofstream html_out(Directories::analysis_request_after_dir);

		if (!html_out.is_open()) {
			Exlog(AMARELO + __func__, VERMELHO + "Nao foi possivel exportar o arquivo html no fstream: html_out");
		}

		html_out << table_html;
		Exlog(AMARELO + __func__, VERDE + "html_out << table_html : concluded.");

		html_out.close();
	}
	catch (const exception& e) {
		// Captura exceções do tipo exception (ou suas derivadas)
		Exlog(AMARELO + __func__, VERMELHO + "Erro durante a chamada (catch): " + e.what());
	}

	Exlog(AMARELO + __func__, EndMsg);

}

void Analysis_Tools::update_analysis_show() {

	Exlog(AMARELO + __func__, StartMsg);

	try {

		//Mapear em AnalysisBase apenas Schemas e Tables que possuem a tabela de analises ja criadas.
		map<string, string> AnalysisBase;

		vector<string> schemas = list_schemas(regex(RegexSeason));

		for (const auto& current_schema : schemas) {

			vector<string> tables = list_tables(current_schema, regex(AnalysisDefaultTable.get<string>()));

			if (!tables.empty()) {
				AnalysisBase[current_schema] = tables[0];
			}

		}

		if (AnalysisBase.empty()) {
			Exlog(AMARELO + __func__, VERMELHO + "Nenhum dado foi coletado no loop for, AnalysisBase");
		}

		Exlog(AMARELO + __func__, VERDE + "Passed first for...");

		//primeiro importamos um pedaço do html + o css completo
		ifstream show_html(Directories::analysis_show_dir);
		if (!show_html.is_open()) {
			Exlog(AMARELO + __func__, VERMELHO + "Nao foi possivel importar o arquivo html no fstream: show_html: " + Directories::analysis_show_dir);
		}

		//começa salvar todas as linhas do arquivo importado na var, table_html
		string table_html{""};
		string line{""};
		bool STOP = false;
		while (getline(show_html, line, '\n')) {

			//quando chegar na parte do codigo com STOP1 pausa a importação
			
			if (regex_search(line, regex(".+START[0-9]+.+"))) {
				//pegamos as informações do banco de dados para criar um codigo html
				Exlog(AMARELO + __func__, AMARELO + "Calling analysis_html_fill | line.c_str: " + line.c_str());
				table_html += MySQL::analysis_html_fill(AnalysisBase, line.c_str());
				STOP = true;
			}
			//quando chegar na parte do codigo com STOP2 volta a permitir a importação
			else if (regex_search(line, regex("--STOP--"))) {
				STOP = false;
			}
			//salva as linhas do html quando permitido
			if (STOP == false) {
				table_html += line + "\n";
			}
		}

		show_html.close();

		ofstream html_out(Directories::analysis_show_after_dir);
		if (!html_out.is_open()) {
			Exlog(AMARELO + __func__, VERMELHO + "Nao foi possivel exportar o arquivo html no fstream: html_out");
		}

		html_out << table_html;
		Exlog(AMARELO + __func__, VERDE + "html_out << table_html : concluded.");	

		html_out.close();
	}
	catch (const exception& e) {
		// Captura exceções do tipo exception (ou suas derivadas)
		Exlog(AMARELO + __func__, VERMELHO + "Erro durante a chamada (catch): " + e.what());
	}

	Exlog(AMARELO + __func__, EndMsg);

}

void Analysis_Tools::operator_menu_main() {

	//atualizar o arquivo html do Analysis Show
	update_analysis_show(); cout << "operator_menu_main 1\n";

	//atualizar o arquivo html do Analysis Request
	update_analysisRequest_show(); cout << "operator_menu_main 2\n";

	AnalysisRoutes(); cout << "operator_menu_main 3\n";

}