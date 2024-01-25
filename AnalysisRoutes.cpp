#include "AnalysisTools.hpp"
#include "DiscordBot.hpp"

#include <iostream>
#include <regex>

//biblioteca crown de rotas
#include <crow.h>
#include "crow/mustache.h"

using namespace TXT_Style;
using namespace std;

void Analysis_Tools::AnalysisRoutes() {

	Exlog("AnalysisRoutes", "CROW_ROUTE", "LOAD", "Current Working Directory: " + filesystem::current_path().string());

	//instanciar objeto crow
	crow::SimpleApp AnalysisApp;

	// Defina o nível de log para DEBUG (ou outro nível desejado)
	crow::logger::setLogLevel(crow::LogLevel::Debug);

	//rota para subir o Operator Menu
	CROW_ROUTE(AnalysisApp, "/menu")([&]() {
		return crow::mustache::load("op_menu.html").render();
	}); Exlog("AnalysisRoutes", "CROW_ROUTE", "LOAD", "/menu");

	//rota para carregar o .html da aba de pedidos de análise.
	CROW_ROUTE(AnalysisApp, "/analysis_request")([] {
		return crow::mustache::load("analysis_request_after.html").render();
	}); Exlog("AnalysisRoutes", "CROW_ROUTE", "LOAD", "/analysis_request");

	//rota para receber dados do js referente a esta aba de pedidos de análise.
	CROW_ROUTE(AnalysisApp, "/analysis_request").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", AMARELO + "Entered on route /analysis_request\n");

		//variavel "req.body" que salva os dados recebidos do html
		const auto data = crow::json::load(req.body);

		if (!data) {
			//Se os dados não puderem ser analisados, retorne um erro
			Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", VERMELHO + "Erro: Dados inválidos.");
			return crow::response(400, "Erro: Dados inválidos.");
		}

		//os dados ficam salvos em "req" e podem ser salvos para modificações aqui:

		//vector que irá armazenar dados enviados do formulário html analysis_assistant
		map<string, string> post_data;


		if (regex_search(string(data["Season"].s()), regex(RegexSeason))) {
			Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "TEST::RESPONSE::post_data[Season] will be -> " + CIANO + string(data["Season"].s()));
			post_data["Season"] = data["Season"].s();
		}
		else {
			Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "NORMAL::RESPONSE::post_data[Season] will be -> " + CIANO + DefaultSeason);
			post_data["Season"] = DefaultSeason;
		}

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", BRANCO + "FINAL::post_data[Season] will be -> " + CIANO + post_data["Season"]);

		post_data["data-do-pedido"]      = data["data-do-pedido"].s();
		post_data["piloto-solicitante"]  = data["piloto-solicitante"].s();
		post_data["piloto-acusado"]      = data["piloto-acusado"].s();
		post_data["simulador"]           = data["simulador"].s();
		post_data["etapa"]               = data["etapa"].s();
		post_data["bateria"]             = data["bateria"].s();
		post_data["minuto-do-incidente"] = data["minuto-do-incidente"].s();
		post_data["link-da-gravacao"]    = data["link-da-gravacao"].s();
		post_data["contexto"]            = data["contexto"].s();


		//procurar pelo schema / table caso não encontrar criar
		Directories::search_in_rpm_base(post_data["Season"], AnalysisDefaultTable.get<string>(), QueryAnalysisValues);

		//função main de envios de análise para o mysql
		MySQL::insert_on_analysis_table(post_data);

		//atualizar o analysis_show. html
		update_analysis_show();

		//enviar mensagem para o discord
		Discord_Tools::discord_enqueue_message(ChannelAnalysisAlert, "@here Novo Pedido de analise: [Data: " + post_data["data-do-pedido"] + "] [Solicitante: " + post_data["piloto-solicitante"] + "] [Acusado: " + post_data["piloto-acusado"] + "] [Etapa: " + post_data["etapa"] + "]");

		return crow::response(200, "Dados recebidos com sucesso.");

	}); Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", "/analysis_request");

	//rota do menu de gerenciamento de analises.
	CROW_ROUTE(AnalysisApp, "/analysis_update").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", AMARELO + "Entered on route /analysis_update\n");

		json data;
		map<string, string> post_data;

		try {

			//fazer o parse do json com o json recebido em POST
			data = json::parse(req.body);

			cout << "\n\n\ndata: " << data << "\n\n\n";

			if (data.is_structured()) {

				Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", VERDE + "O parsing foi bem-sucedido e o JSON é válido.");

				//função que retorna os dados necessarios da analise para punishments.
				post_data = Analysis_Tools::PickPunishmentBySituation(data);
				
				Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", AMARELO + "Calling MySQL::insertOnStageTable");

				MySQL::insertOnStageTable(make_pair(stoi(post_data["etapa"]), stoi(post_data["bateria"])), post_data["piloto_acusado"], stoi(post_data["Punicao_Pontos"]), stoi(post_data["Punicao_Tempo"]), post_data["Punicoes_Adicionais"]);

				cout << "passed function : insertOnStageTable\n\n";

			}
			else {
				Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", VERMELHO + "O parsing foi bem-sucedido, mas o JSON não é válido.");
			}

			Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", AMARELO + "Calling MySQL::insert_on_analysis_table");
			Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", AMARELO + "post_data argument:");
			
			cout << "\n\npost_data:\n";

			for (auto& [key, value] : post_data) {
				cout << key << " : " << value << "\n";
			}

			//so rodar isso se a parte a cima tiver dado certo
			MySQL::insert_on_analysis_table(post_data);

			(post_data["reincidencia"] == "true") ? post_data["reincidencia_discord"] = "Sim" : post_data["reincidencia_discord"] = "Nao";

			string aa_show_msg = R"(
			> :pencil: Pedido de Analise ID: )" + post_data["AnalysisId"] + R"(
			> 
			> :calendar: Temporada: )" + post_data["Season"] + R"(
			> :checkered_flag: Etapa: )" + post_data["etapa"] + R"( - Bateria: )" + post_data["bateria"] + R"(
			> :stopwatch: Minuto: )" + data["momento"].get<string>() + R"(
            > 
			> Piloto solicitante: )" + data["p_solicitante"].get<string>() + R"( - Piloto Acusado: )" + data["p_acusado"].get<string>() + R"(
			> 
			> Situacao Principal: )" + post_data["sit-principal"] + R"(
			> Situacao Intrinseca: )" + post_data["sit-intrinseca"] + R"(
			> 
			> :information_source: Resumo: )" + data["Resume"].get<string>() + R"(
			> 
			> Tipo de Punicao: )" + post_data["SIPunicoes"] + R"(
			> Reincidencia: )" + post_data["reincidencia_discord"] + R"(
			> 
			> Status do pedido: :white_check_mark: Resolvida)";

			Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", AMARELO + "Calling Discord_Tools::discord_enqueue_message");

			//enviar mensagem para o discord
			Discord_Tools::discord_enqueue_message(ChannelAnalysisShow, aa_show_msg);

			//

		}
		catch (const json::parse_error& e) {

			Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", VERMELHO + "Erro ao fazer parsing do JSON: " + e.what());
		}

		return crow::response(200, R"({"message": "[CROW_ROUTE] Dados atualizados com sucesso."})");

	}); Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", "/analysis_update");

	//rota que configura aplicação de punições
	CROW_ROUTE(AnalysisApp, "/punishment_apply").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", AMARELO + "Entered on route /punishment_apply\n");

		auto data = crow::json::load(req.body);

		if (!data) {
			//Se os dados não puderem ser analisados, retorne um erro
			Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", VERMELHO + "Erro: Dados invalidos.");
			return crow::response(400, "Erro: Dados invalidos.");
		}

		//inicialização das variaveis
		string piloto_punido{ "none" }; string etapa{ "none" }; string bateria{ "none" }; string situacaoPrincipal{ "" }; string situacaoIntrinseca{ "" }; bool reincidencia{ false };

		//coletar os dados do indice campo_temp recebido no javascript
		string campo_temp = data["campo_temp"].s(); Exlog(AMARELO + __func__, "json::campo_temp: " + campo_temp);

		//dividir os dados de campo temp (temporada / etapa / bateria)
		smatch sma;
		regex_search(campo_temp, sma, regex("(T\\d{1,3})E(\\d{1,2})R?(\\d)?"));

		string temporada = sma[1].str();
		etapa = sma[2].str();
		bateria = sma[3].str();

		//se nada for encontrado entao a etapa tem bateria unica
		if (bateria.empty()) bateria = "1";

		piloto_punido = data["piloto_punido"].s();
		situacaoPrincipal = data["situacaoPrincipal"].s();
		situacaoIntrinseca = data["situacaoIntrinseca"].s();
		reincidencia = data["reincidencia"].b();

		cout << "\n\nsma\n\n";
		Exlog(AMARELO + __func__, "piloto_punido: " + piloto_punido);
		Exlog(AMARELO + __func__, "json::sma[1]: " + temporada);
		Exlog(AMARELO + __func__, "json::sma[2]: " + etapa);
		Exlog(AMARELO + __func__, "json::sma[3]: " + bateria);


		cout << "\n\nExlog::x\n\n";
		Exlog(AMARELO + __func__, "json::situacaoPrincipal: " + situacaoPrincipal); Exlog(AMARELO + __func__, "json::situacaoIntrinseca: " + situacaoIntrinseca); Exlog(AMARELO + __func__, "json::reincidencia: " + to_string(reincidencia));
		
		//Monta o par com etapa e bateria coletados no regex_search
		pair<int,int> etapa_bateria = make_pair(stoi(etapa), stoi(bateria));

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", AMARELO + "Calling Analysis_Tools::PickPunishmentBySituation");
		//função que retorna os dados para a punição com base nas situações passadas
		map<string,string> post_data = Analysis_Tools::PickPunishmentBySituation(piloto_punido, situacaoPrincipal, situacaoIntrinseca, reincidencia);

		Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", AMARELO + "Calling MySQL::insertOnStageTable.");
		//função que envia os dados para o MySQL
		MySQL::insertOnStageTable(etapa_bateria, piloto_punido, stoi(post_data["Punicao_Pontos"]), stoi(post_data["Punicao_Tempo"]), post_data["Punicoes_Adicionais"]);

		return crow::response(200, R"({"message": "Dados recebidos com sucesso."})");

	}); Exlog("AnalysisRoutes", "CROW_ROUTE", "POST", "/punishment_apply");

	//rota para subir as tabelas de classificação
	CROW_ROUTE(AnalysisApp, "/tabelas")([&]() {
		return crow::mustache::load("table_after.html").render();
	}); Exlog("AnalysisRoutes", "CROW_ROUTE", "LOAD", "table_after.html");

	//Rota para servir links de downloads e outras informações.
	CROW_ROUTE(AnalysisApp, "/rpmdownloader").methods("GET"_method) ([&]() {

		cout << "\n\n/rpmdownloader -- initializing endpoint\n\n";

		if (DownloaderBase.empty()) {
			crow::response res(404);
			res.set_header("Content-Type", "text/plain");
			res.body = "Nenhum dado disponível";
			return res;
		}

		crow::response res(200);
		res.set_header("Content-Type", "text/plain");
		res.body = DownloaderBase;

		cout << "\n\n/rpmdownloader -- returning response\n\n";

		return res;

	}); Exlog("AnalysisRoutes**", "CROW_ROUTE", "LOAD", "/rpmdownloader");

	// Rota para servir arquivos HTML, CSS e JS
	CROW_ROUTE(AnalysisApp, "/<path>").methods(crow::HTTPMethod::GET, crow::HTTPMethod::HEAD)([](const string& filename) {

		try {

			Exlog("AnalysisRoutes", "CROW_ROUTE", "LOAD", "front_end::filename: " + filename);

			string text = Directories::import_file(Directories::templates_dir + "/" + filename).first;

			Exlog("AnalysisRoutes", "CROW_ROUTE", "LOAD", BRANCO + "Size of file " + CIANO + filename + BRANCO +  ": " + CIANO + to_string(text.size()) + " bytes");

			crow::response resp{ text };

			// Verifica a extensão do arquivo para definir o tipo de conteúdo
			if (filename.ends_with(".css")) {
				resp.set_header("Content-Type", "text/css");
			}
			else if (filename.ends_with(".js")) {
				resp.set_header("Content-Type", "application/javascript");
			}
			else if (filename.ends_with(".html")) {
				resp.set_header("Content-Type", "text/html");
			}
			else if (filename.ends_with(".ico")){
				resp.set_header("Content-Type", "image/x-icon");
			}
			else{
				resp.set_header("Content-Type", "text/plain");
			}

			//cout << "\n\n\n/<path> return resp :: 'text': " + text +"\n\n\n";

			return resp;
		}
		catch (const exception& e) {

			cout << "\n\n\n/<path> on catch\n\n\n";

			Exlog("AnalysisRoutes", "CROW_ROUTE", "LOAD", VERMELHO + "Exception caught: " + e.what());
			return crow::response(500, "Internal Server Error");
		}


	}); Exlog("AnalysisRoutes", "CROW_ROUTE", "LOAD", "/<path>");

	Directories::AnalysisRoutesOnEnd = true;

	//executar todas as rotas crow
	AnalysisApp.port(8085).run();

	Exlog(AMARELO + __func__, VERMELHO + ">>>> Crow Route - AnalysisApp finalizado. <<<<");
	system("pause");
	
}