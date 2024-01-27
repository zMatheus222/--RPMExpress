#include "mysql_functions.hpp"
#include "AnalysisTools.hpp"

#include <regex>

using namespace std;
using namespace TXT_Style;
using namespace MySQL;

string MySQL::analysisRequest_html_fill(const char* START) {

	string html{ "" };

	Exlog("INFO", MAGENTA + __func__, StartMsg);

	if (regex_search(START, regex("START1"))) {

		Exlog("INFO", MAGENTA + __func__, BRANCO + "START == " + START);

		con->setSchema(SchemaAllDrivers);

		Exlog("INFO", MAGENTA + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + "SELECT * FROM " + string(TableAllDrivers) + ";");

		pstmt = con->prepareStatement("SELECT * FROM " + string(TableAllDrivers) + ";");
		result = pstmt->executeQuery();

		while (result->next()) {

			//mostrar no analysis_request apenas pilotos da temporada default (atual)
			if (result->getString(7) == DefaultSeason) {
				Exlog("INFO", MAGENTA + __func__, BRANCO + "Piloto " + CIANO + result->getString(5) + BRANCO + " é da temporada atual.");
				html += "\t\t\t\t\t\t<option value='" + result->getString(5) + "'>" + result->getString(5) + "</option>\n";
			}
			else {
				Exlog("INFO", MAGENTA + __func__, BRANCO + "Piloto " + CIANO + result->getString(5) + BRANCO + " não é da temporada atual.");
			}

		}

	}

	if (regex_search(START, regex("START2"))) {

		Exlog("INFO", MAGENTA + __func__, BRANCO + "START == " + START);

		html += "\t\t\t\t\t<input type='text' id='season' name = 'temporada' placeholder = 'Temporada' value = '" + DefaultSeason + "'>";
	}

	return html;
}

string MySQL::analysis_html_fill(const map<string, string>& AnalysisBase, const char* START) {

	string html{ "" };

	Exlog("INFO", MAGENTA + __func__, StartMsg);

	if (regex_search(START, regex("START1"))) {

		Exlog("INFO", MAGENTA + __func__, BRANCO + "START == " + START);

		//adicioar abas de temporada
		html += "\t<div class='season-selector'>\n";
		html += "\t\t<button onclick='toggleSeasons(21)'>Selecione a Temporada</button>\n";
		html += "\t\t<div class='season-dropdown' id='seasonDropdown'>\n";

		//definir as abas de temporada

		auto all_schemas = list_schemas(regex("T\\d{1,3}"));

		for (const auto& current_schema : all_schemas) {
			html += "\t\t\t<a href='#' onclick='toggleSeasons(\"" + current_schema + "\")'>" + current_schema + "</a>\n";
		}

		html += "\t\t</div>\n";
		html += "\t\t<button id='btn-apply-punishiment' class='btn-apply-punishiment'>Aplicar punicao</button>\n";
		html += "\t</div>\n";

		Exlog("INFO", MAGENTA + __func__, BRANCO + "AnalysisBase size: " + CIANO + to_string(AnalysisBase.size()));

		for (const auto& current_schema : all_schemas) {

			Exlog("INFO", MAGENTA + __func__, BRANCO + "Entered on big for, schema.first: " + CIANO + current_schema);

			html += "<table class='season_table' id = '" + current_schema + "'>\n";
			html += "\t<tr>\n";
			html += "\t\t<th>Id</th>\n";
			html += "\t\t<th>Data do Pedido</th>\n";
			html += "\t\t<th>Piloto Solicitante</th>\n";
			html += "\t\t<th>Piloto Acusado</th>\n";
			html += "\t\t<th>Simulador</th>\n";
			html += "\t\t<th>Etapa</th>\n";
			html += "\t\t<th>Bateria</th>\n";
			html += "\t\t<th>Minuto</th>\n";
			html += "\t\t<th>Link Gravacao</th>\n";
			html += "\t\t<th>Status</th>\n";
			html += "\t</tr>\n";

			con->setSchema(current_schema);

			Exlog("INFO", MAGENTA + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + "SELECT * FROM " + string(AnalysisDefaultTable) + ";");

			pstmt = con->prepareStatement("SELECT * FROM " + string(AnalysisDefaultTable) + ";");
			result = pstmt->executeQuery();

			Exlog("INFO", MAGENTA + __func__, BRANCO + "Query on Schema: " + CIANO + current_schema + BRANCO + " (pstmt): " + CIANO + "SELECT * FROM: " + string(AnalysisDefaultTable));

			while (result->next()) {

				//definimos se a punição está pendente ou resolvida com base se a coluna 13 da linha (punição) tem algum valor ou não.

				string punishment_status;
				string punishment_color;

				if (regex_search(result->getString(13).c_str(), regex("^$"))) {
					punishment_status = "Pendente";
					punishment_color = "amarelo";
				}
				else {
					punishment_status = "Resolvida";
					punishment_color = "verde";
				}

				string db_str = result->getString(9);
				string record_link = validate_link(db_str);

				string button_link = "<button class='btn_record' onclick=\"window.open('" + record_link + "', '_blank')\">Acessar</button>";

				html += R"(
		<tr class='main-row'>
			<td>)" + result->getString(1) + R"(</td>
			<td>)" + result->getString(2) + R"(</td>
			<td id="p_solicitante">)" + result->getString(3) + R"(</td>
			<td id="p_acusado">)" + result->getString(4) + R"(</td>
			<td>)" + result->getString(5) + R"(</td>
			<td id="etapa">)" + result->getString(6) + R"(</td>
			<td id="bateria">)" + result->getString(7) + R"(</td>
			<td id="momento">)" + result->getString(8) + R"(</td>
			<td>)" + button_link + R"(</td>
			<td id="a_status"><button class='btn-)" + punishment_color + R"('>)" + punishment_status + R"(</button></td>
		</tr>
		<tr class='details-row'>
			<td colspan='9'>
				<div class='details-container'>
					<!-- Seus detalhes adicionais -->
					<div><strong>Contexto: </strong>)" + result->getString(10) + R"(</div>
					<div><strong>Situacao Principal: </strong>)" + result->getString(11) + R"(</div>
					<div><strong>Situacao Intrinseca: </strong>)" + result->getString(12) + R"(</div>
					<div><strong>Punicao de Tempo: </strong>)" + result->getString(13) + R"(</div>
					<div><strong>Punicao de Pontos: </strong>)" + result->getString(14) + R"(</div>
					<div><strong>Punicoes Adicionais: </strong>)" + result->getString(15) + R"(</div>
				</div>
			</td>
		</tr>

	)";
			}

			html += "</table>\n";
		}

		delete pstmt;
		delete result;
	}

	else if (regex_search(START, regex("START2"))) {

		for (const auto& schema : list_schemas(regex("T\\d{1,3}"))) {

			int count{ 0 };
			for (const auto& table : list_tables(schema, regex("E[0-9]+_.+"))) {

				smatch sma;
				regex_search(table, sma, regex("(E([0-9]+))_(.+)_(R([0-9]?))"));

				const string EWithNumber = sma[1].str();
				const string ENumber = sma[2].str();
				const string EName = sma[3].str();

				const string RWithNumber = sma[4].str();

				string BNumber;
				(sma[5].str().empty()) ? BNumber = "" : BNumber = "R" + sma[5].str();

				html += "\t\t\t\t<option value = '" + schema + EWithNumber + BNumber + "'>" + schema + EWithNumber + BNumber + "</option>\n";

				count++;
			}

		}

	}
	else if (regex_search(START, regex("START3"))) {

		con->setSchema(SchemaAllDrivers);

		Exlog("INFO", MAGENTA + __func__, BRANCO + "getSchema on: " + CIANO + con->getSchema().asStdString());

		stmt = con->createStatement();
		stmt->execute("SELECT id, playerId, lastName FROM " + string(TableAllDrivers) + ";");

		result = stmt->getResultSet();

		while (result->next()) {
			html += "\t\t\t\t<option value='" + result->getString(2) + "'>" + result->getString(3) + "</option>\n";
		}

	}

	Exlog("INFO", MAGENTA + __func__, EndMsg);

	return html;

}

void MySQL::insert_on_analysis_table(map<string, string>& post_data) {

	Exlog("INFO", AMARELO + __func__, StartMsg); Exlog("INFO", AMARELO + __func__, AMARELO + "Verificando se o schema " + CIANO + post_data["Season"] + AMARELO + " solicitado existe...");

	Directories::search_in_rpm_base(post_data["Season"], AnalysisDefaultTable, QueryAnalysisValues);

	//escolher o schema passado.
	con->setSchema(post_data["Season"]);

	Exlog("INFO", AMARELO + __func__, BRANCO + "Realizando setSchema em: " + CIANO + con->getSchema().asStdString());

	Exlog("INFO", AMARELO + __func__, BRANCO + "post_data::list:");

	for (const pair<string, string>& data : post_data) {
		Exlog("INFO", AMARELO + __func__, BRANCO + "first: " + CIANO + data.first + BRANCO + " | second: " + CIANO + data.second);
	}

	try {

		// se encontrarmos um valor chave de simulador, entrar aqui.
		if (post_data.find("simulador") != post_data.end()) {
			//essa função deve ser usada quando o usuário no site cria um pedido de analise. os dados passam por essa função inserindo esses dados.

			Exlog("INFO", MAGENTA + __func__, BRANCO + "Realizando UPDATE da analise na table.");
			Exlog("INFO", MAGENTA + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + QueryAnalysisInsert);

			pstmt = con->prepareStatement(QueryAnalysisInsert);

			Exlog("INFO", MAGENTA + __func__, BRANCO + "Realizando INSERT na table " + AnalysisDefaultTable.dump() + " da temporada atual.");

			pstmt->setString(1, post_data["data-do-pedido"]);
			pstmt->setString(2, post_data["piloto-solicitante"]);
			pstmt->setString(3, post_data["piloto-acusado"]);
			pstmt->setString(4, post_data["simulador"]);
			pstmt->setInt(5, stoi(post_data["etapa"]));
			pstmt->setInt(6, stoi(post_data["bateria"]));
			pstmt->setString(7, post_data["minuto-do-incidente"]); //minuto
			pstmt->setString(8, post_data["link-da-gravacao"]);
			pstmt->setString(9, post_data["contexto"]);

			pstmt->execute();

		}
		// se encontrarmos um valor chave de punição, entrar aqui.
		else if (post_data.find("Punicao_Tempo") != post_data.end()) {

			//essa função deve ser usada quando um operador resolver uma analise, os dados de punição serão aplicados aqui.

			Exlog("INFO", MAGENTA + __func__, BRANCO + "Realizando INSERT das punicoes da analise id: " + CIANO + post_data["AnalysisId"] + BRANCO + " na table.");

			pstmt = con->prepareStatement(QueryPunishmentInsert);

			Exlog("INFO", MAGENTA + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + QueryPunishmentInsert);

			pstmt->setString(1, post_data["sit-principal"]);
			pstmt->setString(2, post_data["sit-intrinseca"]);
			pstmt->setInt(3, stoi(post_data["Punicao_Tempo"]));
			pstmt->setInt(4, stoi(post_data["Punicao_Pontos"]));
			pstmt->setString(5, post_data["Punicoes_Adicionais"]);
			pstmt->setString(6, post_data["AnalysisId"]);

			pstmt->execute();
			Exlog("INFO", MAGENTA + __func__, BRANCO + "Realizado Statment (pstmt). AnalysisId: " + CIANO + post_data["AnalysisId"]);


			Exlog("INFO", MAGENTA + __func__, BRANCO + "Inserindo valores de punicao da analise na table de resultados.");

			cout << "etapa: " + post_data["etapa"] << "\n";
			cout << "bateria: " + post_data["bateria"] << "\n";
			cout << "piloto_acusado: " + post_data["piloto_acusado"] << "\n";
			cout << "Punicao_Pontos: " + post_data["Punicao_Pontos"] << "\n";
			cout << "Punicao_Tempo: " + post_data["Punicao_Tempo"] << "\n";
			cout << "Punicoes_Adicionais: " + post_data["Punicoes_Adicionais"] << "\n";

			MySQL::insertOnStageTable(make_pair(stoi(post_data["etapa"]), stoi(post_data["bateria"])), post_data["piloto_acusado"], stoi(post_data["Punicao_Pontos"]), stoi(post_data["Punicao_Pontos"]), post_data["Punicoes_Adicionais"]);

		}
		else {
			Exlog("ERROR", MAGENTA + __func__, VERMELHO + "Algum problema durante a verificacao if post_data.find()");
		}
	}
	catch (sql::SQLException& e) {

		Exlog("ERROR", MAGENTA + __func__, VERMELHO + "Problema durante o INSERT das analises no MySQL!");
		Exlog("ERROR", MAGENTA + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog("ERROR", MAGENTA + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
		Exlog("ERROR", MAGENTA + __func__, VERMELHO + "SQLState: " + e.getSQLState());

	}

	delete pstmt;

	Exlog("INFO", MAGENTA + __func__, VERDE + "Realizado procedimento no MySQL, atualizando analysis show.");

	Analysis_Tools::update_analysis_show();

	Exlog("INFO", AMARELO + __func__, EndMsg);

}

//inserir na tabela referente a etapa atual os dados de analise.
void MySQL::insertOnStageTable(pair<int, int> etapa_bateria, string piloto_acusado, int Punicao_Pontos, int Punicao_Tempo, string Punicoes_Adicionais) {

	Exlog("INFO", AMARELO + __func__, StartMsg);

	cout << "\n\n\netapa_bateria.first: " + to_string(etapa_bateria.first) << "\n";
	cout << "etapa_bateria.second: " + to_string(etapa_bateria.second) << "\n";

	int etapa = etapa_bateria.first;
	int bateria = etapa_bateria.second;

	con->setSchema(DefaultSeason);

	Exlog("INFO", AMARELO + __func__, BRANCO + "Realizando setSchema em: " + CIANO + con->getSchema().asStdString());

	try {
		//buscar tabela com base na etapa e bateria
		string rgx_etapa_bateria = "E" + to_string(etapa) + ".+_R" + to_string(bateria) + "?"; Exlog("INFO", AMARELO + __func__, BRANCO + "rgx_etapa_bateria: " + CIANO + rgx_etapa_bateria);
		vector<string> table = list_tables(DefaultSeason, regex(rgx_etapa_bateria));

		string needed_table = table[0];

		Exlog("INFO", AMARELO + __func__, BRANCO + "buscando piloto: " + CIANO + piloto_acusado + BRANCO + " na table: " + CIANO + needed_table);

		//buscar id com base no nome do piloto acusado na tabela da etapa //essa id nao é o steam guid mas sim a chave primaria da tabela
		int playerKeyId = findDriverOnTable(needed_table, piloto_acusado);

		Exlog("INFO", AMARELO + __func__, BRANCO + "needed_table: " + CIANO + needed_table);

		Exlog("INFO", AMARELO + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + QueryStageUpdate(needed_table));

		pstmt = con->prepareStatement(QueryStageUpdate(needed_table));

		Exlog("INFO", AMARELO + __func__, BRANCO + "Realizando INSERT na table " + needed_table + " da temporada atual, id: " + to_string(playerKeyId));

		pstmt->setInt(1, Punicao_Pontos);
		pstmt->setInt(2, Punicao_Tempo);
		pstmt->setString(3, Punicoes_Adicionais);
		pstmt->setInt(4, playerKeyId);

		pstmt->execute();

		Exlog("INFO", AMARELO + __func__, VERDE + "Realizado Statement (pstmt).");


		//transformar punição de tempo em millisegundos e somar ao totaltime do piloto, reclassificar tabela

		int Punicao_Tempo_ms = Punicao_Tempo * 1000;

		Exlog("INFO", AMARELO + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + "UPDATE " + needed_table + " SET totalTime = totalTime + " + to_string(Punicao_Tempo_ms) + " WHERE id = " + to_string(playerKeyId) + ";");
		pstmt = con->prepareStatement("UPDATE " + needed_table + " SET totalTime = totalTime + " + to_string(Punicao_Tempo_ms) + " WHERE id = " + to_string(playerKeyId) + ";");

		pstmt->executeUpdate();

	}
	catch (sql::SQLException& e) {
		Exlog("ERROR", AMARELO + __func__, VERMELHO + "Problema durante o INSERT das analises no MySQL!");
		Exlog("ERROR", AMARELO + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog("ERROR", AMARELO + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
		Exlog("ERROR", AMARELO + __func__, VERMELHO + "SQLState: " + e.getSQLState());
	}

}
