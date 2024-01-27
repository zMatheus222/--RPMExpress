#include "mysql_functions.hpp"

using namespace std;
using namespace TXT_Style;
using namespace MySQL;

void MySQL::update_geral_ranking() {
	Exlog(AMARELO + __func__, StartMsg);

	//fazer update da tabela geral

	try {

		vector<string> schemas = list_schemas(regex("T\\d{1,3}"));

		for (const auto& schema : schemas) {

			Exlog(AMARELO + __func__, BRANCO + "schema: " + CIANO + schema);

			con->setSchema(schema); Exlog(AMARELO + __func__, BRANCO + "getSchema on: " + CIANO + con->getSchema().asStdString());

			create_table(schema, "Tabela_Geral", false, TabelaGeralIndex); Exlog(AMARELO + __func__, BRANCO + "Tabela_Geral criada.");

			//pegar todas as tabelas da etapa atual

			vector<string> tables = list_tables(schema, regex("E[0-9]+_.+")); Exlog(AMARELO + __func__, BRANCO + "tables.size(): " + CIANO + to_string(tables.size()));

			cout << "\n";

			string etapa_bateria{ "" };
			map<string, string> tableData;

			for (const string& current_table : tables) {
				Exlog(AMARELO + __func__, BRANCO + "current_table: " + CIANO + current_table);

				pstmt = con->prepareStatement("SELECT * FROM " + current_table + "; "); //Exlog(AMARELO + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + "SELECT * FROM " + current_table + "; ");

				result = pstmt->executeQuery(); //Exlog(AMARELO + __func__, BRANCO + "Realizando SELECT na table " + current_table + " da temporada atual.");

				while (result->next()) {

					tableData["playerId"] = result->isNull(1) ? "NULL" : ("'" + result->getString(2) + "'");
					tableData["cupCategory"] = result->isNull(2) ? "NULL" : to_string(result->getInt(3));
					tableData["raceNumber"] = result->isNull(3) ? "NULL" : to_string(result->getInt(4));
					tableData["shortName"] = result->isNull(4) ? "NULL" : ("'" + result->getString(5) + "'");
					tableData["firstName"] = result->isNull(5) ? "NULL" : ("'" + result->getString(6) + "'");
					tableData["lastName"] = result->isNull(6) ? "NULL" : ("'" + result->getString(7) + "'");
					tableData["teamName"] = result->isNull(7) ? "NULL" : ("'" + result->getString(8) + "'");
					tableData["carModel"] = result->isNull(8) ? "NULL" : ("'" + result->getString(9) + "'");
					tableData["lapCount"] = result->isNull(9) ? "NULL" : to_string(result->getInt(10));
					tableData["bestLap"] = result->isNull(10) ? "NULL" : to_string(result->getInt(11));
					tableData["totalTime"] = result->isNull(11) ? "NULL" : to_string(result->getInt(12));
					tableData["Punicao_Tempo"] = result->isNull(12) ? "NULL" : to_string(result->getInt(13));
					tableData["Punicao_Pontos"] = result->isNull(13) ? "NULL" : to_string(result->getInt(14));
					tableData["Punicoes_Adicionais"] = result->isNull(14) ? "NULL" : ("'" + result->getString(15) + "'");

					//pegar o nome da etapa e bateria da tabela atual

					smatch sma; regex_search(current_table, sma, regex("(E([0-9]+))_(.+)_(R([0-9]?))"));

					const string EWithNumber = sma[1].str(); string BNumber;
					(sma[5].str().empty()) ? BNumber = "B1" : BNumber = "B" + sma[5].str();

					etapa_bateria = EWithNumber + BNumber;

					tableData[etapa_bateria] = to_string(result->getInt(16));	//cout << "result->getInt(16): " << result->getInt(16) << "\n";

					//Exlog(AMARELO + __func__, BRANCO + "etapa_bateria: " + CIANO + etapa_bateria);
					//Exlog(AMARELO + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + TableGeralInsert(tableData, etapa_bateria));

					pstmt = con->prepareStatement(TableGeralInsert(tableData, etapa_bateria));

					pstmt->execute();

				}

			}

		}

	}
	catch (sql::SQLException& e) {
		Exlog(AMARELO + __func__, VERMELHO + "Problema durante o INSERT das analises no MySQL!");
		Exlog(AMARELO + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog(AMARELO + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
		Exlog(AMARELO + __func__, VERMELHO + "SQLState: " + e.getSQLState());
	}

	Exlog(AMARELO + __func__, EndMsg);

}

string MySQL::results_html_fill(vector<string>& schemas, const char* START) {

	Exlog(MAGENTA + __func__, StartMsg);

	string html{ "" };

	if (regex_search(START, regex("START1"))) {

		Exlog(MAGENTA + __func__, BRANCO + "START == " + START);

		for (const auto& current_schema : schemas) {
			html += "\t\t\t\t<option value='" + current_schema + "'>" + current_schema + "</option>\n";
			Exlog(MAGENTA + __func__, BRANCO + "current_schema: " + CIANO + current_schema);
		}
	}
	else if (regex_search(START, regex("START2"))) {

		Exlog(MAGENTA + __func__, BRANCO + "START == " + START);

		for (const string& current_schema : schemas) {

			vector<string> tables = list_tables(current_schema, regex("(?:E[0-9]+_.+)|(?:Tabela_Geral)"));

			Exlog(MAGENTA + __func__, BRANCO + "tables: ");

			for (const auto& current_table : tables) {

				cout << "\n\n" + VERDE + "current_table: " << current_table << "\n\n";

				smatch sma;

				if (regex_search(current_table, sma, regex("(E([0-9]+))_(.+)_(R([0-9]?))"))) {

					const string EWithNumber = sma[1].str();
					const string ENumber = sma[2].str();
					const string EName = sma[3].str();

					const string RWithNumber = sma[4].str();

					string BNumber;
					(sma[5].str().empty()) ? BNumber = "" : BNumber = " B" + sma[5].str();

					html += "\t\t\t\t<option class='table_dropdown' value='" + current_schema + EWithNumber + RWithNumber + "'>" + EWithNumber + " - " + EName + BNumber + "</option>\n";

					//cout << html;
				}
				else if (regex_search(current_table, sma, regex("Tabela_Geral"))) {

					Exlog(MAGENTA + __func__, "Entered on regex_search Tabela_Geral");

					Exlog(MAGENTA + __func__, BRANCO + "table: " + CIANO + sma[1].str() + sma[4].str());

					html += "\t\t\t\t<option class='table_dropdown' value='" + current_schema + "Tabela_Geral" + "'>Tabela Geral</option>\n";

					Exlog(MAGENTA + __func__, BRANCO + "html: " + CIANO + html);

				}

			}
		}
	}
	else if (regex_search(START, regex("START3"))) {

		Exlog(MAGENTA + __func__, CIANO + START);

		for (string& current_schema : schemas) {

			vector<string> tables = list_tables(current_schema, regex("Tabela_Geral"));

			con->setSchema(current_schema);
			Exlog(MAGENTA + __func__, BRANCO + "getSchema on: " + CIANO + con->getSchema().asStdString());

			for (auto& current_table : tables) {

				if (current_table != "none") {

					Exlog(MAGENTA + __func__, BRANCO + "current_table: " + CIANO + current_table);

					//se a tabela não for uma tabela de analises execute.
					if (!regex_search(current_table, regex(".*Analysis.*"))) {

						html += R"(
			<table class="tabela_temporada" id=')" + current_schema + R"('>

			<table class='tabela_etapa' id=')" + current_schema + current_table + R"('>

				<th class='TG-Short' id='cupCategory'>Classe</th>
				<th class='TG-Short' id='raceNumber'>Number</th>
				<th class='TG-Short' id='shortName'>Tag</th>
				<th class='TG-Long' id='Nome'>Nome</th>
				<th class='TG-Long' id='Equipe'>Equipe</th>
				<th class='TG-Long' id='Model'>Modelo</th>
                <th class='TG-Short' id='Punicao_Tempo'>P_Tempo</th>
                <th class='TG-Short' id='Punicao_Pontos'>P_Pontos</th>
                <th class='TG-Short' id='Punicoes_Adicionais'>P_Extra</th>
                <th class='TG-Short' id='E1B1'>E1B1</th>
                <th class='TG-Short' id='E1B2'>E1B2</th>
                <th class='TG-Short' id='E2B1'>E2B1</th>
                <th class='TG-Short' id='E2B2'>E2B2</th>
                <th class='TG-Short' id='E3B1'>E3B1</th>
                <th class='TG-Short' id='E3B2'>E3B2</th>
                <th class='TG-Short' id='E4B1'>E4B1</th>
                <th class='TG-Short' id='E4B2'>E4B2</th>
                <th class='TG-Short' id='E5B1'>E5B1</th>
                <th class='TG-Short' id='E5B2'>E5B2</th>
				<th class='TG-Short' id='TotalPontos'>Pontos</th>
			)";

						Exlog(MAGENTA + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + TableGeralSelect(current_schema));

						pstmt = con->prepareStatement(TableGeralSelect(current_schema));
						result = pstmt->executeQuery();

						while (result->next()) {

							html += R"(
				<tr>
					<td>)" + to_string(result->getInt(1)) + R"(</td>
					<td>)" + to_string(result->getInt(2)) + R"(</td>
					<td>)" + result->getString(3).asStdString() + R"(</td>
					<td>)" + result->getString(4).asStdString() + R"(</td>
					<td>)" + result->getString(5).asStdString() + R"(</td>
					<td>)" + to_string(result->getInt(6)) + R"(</td>
					<td>)" + to_string(result->getInt(7)) + R"(</td>
					<td>)" + result->getString(8).asStdString() + R"(</td>
					<td>)" + to_string(result->getInt(9)) + R"(</td>
					<td>)" + to_string(result->getInt(10)) + R"(</td>
					<td>)" + to_string(result->getInt(11)) + R"(</td>
					<td>)" + to_string(result->getInt(12)) + R"(</td>
					<td>)" + to_string(result->getInt(13)) + R"(</td>
					<td>)" + to_string(result->getInt(14)) + R"(</td>
					<td>)" + to_string(result->getInt(15)) + R"(</td>
					<td>)" + to_string(result->getInt(16)) + R"(</td>
					<td>)" + to_string(result->getInt(17)) + R"(</td>
					<td>)" + to_string(result->getInt(18)) + R"(</td>
					<td>)" + to_string(result->getInt(19)) + R"(</td>
					<td>)" + to_string(result->getInt(20)) + R"(</td>
				</tr>
							)";
						}

						html += R"(</table>)";

					}
				}
				else {
					Exlog(AMARELO + __func__, AMARELO + "tablelist 'none' pulando para o próximo schema.");
					break;
				}
			}
		}
	}
	else if (regex_search(START, regex("START4"))) {

		Exlog(MAGENTA + __func__, CIANO + START);

		//para cada schema, adicionar as tabelas dele no html.
		for (string& current_schema : schemas) {

			vector<string> tables = list_tables(current_schema, regex("E[0-9]+_.+"));

			con->setSchema(current_schema);
			Exlog(MAGENTA + __func__, BRANCO + "getSchema on: " + CIANO + con->getSchema().asStdString());

			//passar de 1 em 1 as tables do database.
			for (auto& current_table : tables) {

				if (current_table != "none") {

					Exlog(MAGENTA + __func__, BRANCO + "current_table: " + CIANO + current_table);

					string E_Number{ "" };

					//pegar os caracteres do nome para gerar um id para o js / html

					smatch sma; regex_search(current_table, sma, regex("(E[0-9]+)_.+_(R[0-9]?)"));

					E_Number = sma[1].str() + sma[2].str();
					//Exlog(MAGENTA + __func__, BRANCO + "sma[1]:" + CIANO + sma[1].str() + BRANCO + " sma[2]:" + CIANO + sma[2].str());

					//E_Number será o identificador da etapa no html exemplo: E1B1, isso será usado para a lógica no javascript.
					//Exlog(MAGENTA + __func__, BRANCO + "E_Number: " + CIANO + E_Number);

					//se a tabela não for uma tabela de analises execute.
					if (!regex_search(current_table, regex(".*Analysis.*"))) {

						//Exlog(MAGENTA + __func__, BRANCO + "current_table: " + CIANO + current_table);

						//Exlog(MAGENTA + __func__, BRANCO + "START == --START2--");

						html += R"(
			<table class="tabela_temporada" id=')" + current_schema + R"('>

			<table class='tabela_etapa' id=')" + current_schema + E_Number + R"('>

				<th class='upd'></th>
				<th class='Pos'>Pos</th>
				<th class='Cat'>Cat</th>
				<th class='Pil'>Piloto</th>
				<th class='Eqp'>Equipe</th>
				<th class='Num'>Num</th>
				<th class='Tag'>Tag</th>
				<th class='Car'>Modelo</th>
				<th class='Pts'>Pts</th>
				<th class='Pun' id="PSec">PSec</th>
                <th class='Pun' id="PTime">PTime</th>
                <th class='Pun' id="PAdic">PAdic</th>
			)";

						pstmt = con->prepareStatement("SELECT * FROM " + current_table + ";");
						result = pstmt->executeQuery();

						while (result->next()) {

							/*
							Exlog(MAGENTA + __func__, BRANCO + "getString(1): " + CIANO + result->getString(1).asStdString()); //position
							Exlog(MAGENTA + __func__, BRANCO + "getInt(2): " + CIANO + to_string(result->getInt(2))); //playerId
							Exlog(MAGENTA + __func__, BRANCO + "getInt(3): " + CIANO + to_string(result->getInt(3))); //cupCategory
							Exlog(MAGENTA + __func__, BRANCO + "getString(4): " + CIANO + result->getString(4).asStdString()); //raceNumber
							Exlog(MAGENTA + __func__, BRANCO + "getString(5): " + CIANO + result->getString(5).asStdString()); //shortname
							Exlog(MAGENTA + __func__, BRANCO + "getString(6): " + CIANO + result->getString(6).asStdString()); //firstName
							Exlog(MAGENTA + __func__, BRANCO + "getString(7): " + CIANO + result->getString(7).asStdString()); //lastName
							Exlog(MAGENTA + __func__, BRANCO + "getString(8): " + CIANO + result->getString(8).asStdString());//teamName
							Exlog(MAGENTA + __func__, BRANCO + "getInt(9): " + CIANO + to_string(result->getInt(9))); //carModel
							Exlog(MAGENTA + __func__, BRANCO + "getInt(10): " + CIANO + to_string(result->getInt(10))); //lapCount
							Exlog(MAGENTA + __func__, BRANCO + "getInt(11): " + CIANO + to_string(result->getInt(11))); //bestLap
							*/

							//Adquirir pontos do arquivo . com base na posição 'getInt(3)'
							int position = stoi(result->getString(1).asStdString());

							//Exlog(MAGENTA + __func__, BRANCO + "Posição: " + result->getString(1).asStdString() + " | Pontos: " + CIANO + to_string(Pontos));

							html += R"(
				<tr>
					<td class="driver_upd">&#9660;</td>
					<td>)" + result->getString(1).asStdString() + R"(</td>
					<td class="driver_class">)" + to_string(result->getInt(3)) + R"(</td>
					<td>)" + result->getString(7).asStdString() + R"(</td>
					<td>)" + result->getString(8).asStdString() + R"(</td>
					<td>)" + result->getString(4).asStdString() + R"(</td>
					<td>)" + result->getString(5).asStdString() + R"(</td>
					<td>)" + to_string(result->getInt(9)) + R"(</td>
					<td>)" + to_string(result->getInt(16)) + R"(</td>
					<td class="driver_punishment">)" + result->getString(13).asStdString() + R"(</td>
                    <td class="driver_punishment">)" + result->getString(14).asStdString() + R"(</td>
                    <td class="driver_punishment">)" + result->getString(15).asStdString() + R"(</td>
				</tr>
							)";
						}

						html += R"(</table>)";

					}
				}
				else {
					Exlog(AMARELO + __func__, AMARELO + "tablelist 'none' pulando para o próximo schema.");
					break;
				}
			}
		}

		delete result;
		delete stmt;
		delete pstmt;


		Exlog(MAGENTA + __func__, EndMsg);

	}

	return html;

}

bool MySQL::verify_existing_content(string& schema, string& table) {
	Exlog(AMARELO + __func__, StartMsg);

	pstmt = con->prepareStatement("SELECT * FROM " + table + ";"); Exlog(AMARELO + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + "SELECT * FROM " + table + ";");

	result = pstmt->executeQuery(); Exlog(AMARELO + __func__, BRANCO + "Query on Schema: " + CIANO + schema + BRANCO + " (pstmt): " + CIANO + "SELECT * FROM: " + table);

	//verificar se existe itens no result, caso sim ele entra no while, senão não, e retorna "none"

	bool have_itens = false;

	while (result->next()) {
		Exlog(AMARELO + __func__, BRANCO + "getString(1): " + CIANO + result->getString(1).asStdString());
		have_itens = true;
		result->beforeFirst();
		break;
	}

	if (!have_itens) {
		Exlog(AMARELO + __func__, VERDE + "Não existe conteúdo na table: " + table + ", Schema: " + schema);
		return false;
	}
	else {
		Exlog(AMARELO + __func__, VERDE + "Existe conteúdo na table: " + table + ", Schema: " + schema);
		return true;
	}

	return false;
}

void MySQL::insert_on_results_table(map<string, string>& tableData) {

	Exlog(AMARELO + __func__, StartMsg);

	Exlog(AMARELO + __func__, BRANCO + "Pre-setSchema " + tableData["Season"]);

	//escolher o schema passado.
	con->setSchema(tableData["Season"]);

	Exlog(AMARELO + __func__, BRANCO + "Realizando setSchema em: " + CIANO + con->getSchema().asStdString());

	try {

		//listar todos os totaltimes | lap counts para depois comparar aqui e enviar os dados na ordem correta
		//corrigir os dados para irem o mais originais possíveis para o MySQL

		Exlog(AMARELO + __func__, BRANCO + "Verificar se já existe conteúdo nessa table");

		//MySQL::verify_existing_content(tableData["Season"], tableData["tableName"], "conteudo");

		Exlog(AMARELO + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + QueryResultInsert(tableData["tableName"]));

		pstmt = con->prepareStatement(QueryResultInsert(tableData["tableName"]));

		Exlog(AMARELO + __func__, BRANCO + "Realizando INSERT na table " + tableData["tableName"] + " da temporada atual.");

		/*
		cout << "\n\n\n------> playerId::" << tableData["playerId"] << "\n";
		cout << "\n------> cupCategory::" << tableData["cupCategory"] << "\n";
		cout << "\n------> raceNumber::" << tableData["raceNumber"] << "\n";
		cout << "\n------> shortName::" << tableData["shortName"] << "\n";
		cout << "\n------> firstName::" << tableData["firstName"] << "\n";
		cout << "\n------> lastName::" << tableData["lastName"] << "\n";
		cout << "\n------> teamName::" << tableData["teamName"] << "\n";
		cout << "\n------> carModel::" << tableData["carModel"] << "\n";
		cout << "\n------> lapCount::" << tableData["lapCount"] << "\n";
		cout << "\n------> bestLap::" << tableData["bestLap"] << "\n";
		cout << "\n------> totalTime::" << tableData["totalTime"] << "\n";
		cout << "\n------> Pontos::" << tableData["Pontos"] << "\n\n\n";
		*/

		pstmt->setString(1, tableData["playerId"]);
		pstmt->setInt(2, stoi(tableData["cupCategory"]));
		pstmt->setInt(3, stoi(tableData["raceNumber"]));
		pstmt->setString(4, tableData["shortName"]);
		pstmt->setString(5, tableData["firstName"]);
		pstmt->setString(6, tableData["lastName"]);
		pstmt->setString(7, tableData["teamName"]);
		pstmt->setInt(8, stoi(tableData["carModel"]));
		pstmt->setInt(9, stoi(tableData["lapCount"]));
		pstmt->setInt(10, stoi(tableData["bestLap"]));
		pstmt->setInt(11, stoi(tableData["totalTime"]));
		pstmt->setInt(12, stoi(tableData["Pontos"]));


		pstmt->execute();

		Exlog(AMARELO + __func__, VERDE + "Realizado Statement (pstmt).");

	}
	catch (sql::SQLException& e) {
		Exlog(AMARELO + __func__, VERMELHO + "Problema durante o INSERT das analises no MySQL!");
		Exlog(AMARELO + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog(AMARELO + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
		Exlog(AMARELO + __func__, VERMELHO + "SQLState: " + e.getSQLState());
	}

}

void insertAllDriversOnMySQL(map<string, vector<string>>& driverData) {

	Exlog(AMARELO + __func__, StartMsg);

	//escolher o schema passado.
	con->setSchema(SchemaAllDrivers);

	Exlog(AMARELO + __func__, BRANCO + "Realizando setSchema em: " + CIANO + con->getSchema().asStdString());

	create_table(SchemaAllDrivers, TableAllDrivers, false, QueryAllDriversValues);

	try {

		Exlog(AMARELO + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + QueryInsertAllDrivers(string(TableAllDrivers)));

		pstmt = con->prepareStatement(QueryInsertAllDrivers(string(TableAllDrivers)));

		Exlog(AMARELO + __func__, BRANCO + "Realizando INSERT na table " + TableAllDrivers);

		for (const auto& driver : driverData) {

			Exlog(AMARELO + __func__, BRANCO + "driver.first: " + CIANO + driver.first);
			Exlog(AMARELO + __func__, BRANCO + "driver.second[0]: " + CIANO + driver.second[0]);
			Exlog(AMARELO + __func__, BRANCO + "driver.second[1]: " + CIANO + driver.second[1]);
			Exlog(AMARELO + __func__, BRANCO + "driver.second[2]: " + CIANO + driver.second[2]);
			Exlog(AMARELO + __func__, BRANCO + "driver.second[3]: " + CIANO + driver.second[3]);
			Exlog(AMARELO + __func__, BRANCO + "driver.second[4]: " + CIANO + driver.second[4]);

			//para inserção na table
			pstmt->setString(1, driver.first);
			pstmt->setString(2, driver.second[0]);
			pstmt->setString(3, driver.second[1]);
			pstmt->setString(4, driver.second[2]);
			pstmt->setString(5, driver.second[3]);
			pstmt->setString(6, driver.second[4]);

			//para consulta (que verifica se os dados já existem na table)
			pstmt->setString(7, driver.first);
			pstmt->setString(8, driver.second[0]);
			pstmt->setString(9, driver.second[1]);
			pstmt->setString(10, driver.second[2]);
			pstmt->setString(11, driver.second[3]);
			pstmt->setString(12, driver.second[4]);

			pstmt->execute();

			Exlog(AMARELO + __func__, VERDE + "Realizado Statement (pstmt).");

		}

	}
	catch (sql::SQLException& e) {
		Exlog(AMARELO + __func__, VERMELHO + "Problema durante o INSERT das analises no MySQL!");
		Exlog(AMARELO + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog(AMARELO + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
	}


}

void Directories::registerAllDrivers() {
	Exlog(AMARELO + __func__, StartMsg);

	create_schema("RPM_Drivers"); Exlog(AMARELO + __func__, BRANCO + "Criado Schema: " + CIANO + "RPM_Drivers");

	create_table("RPM_Drivers", "All_Drivers", false, QueryAllDriversValues);

	//listar todos os schemas
	vector<string> schemas = MySQL::list_schemas(regex("T\\d{1,3}"));

	map<string, vector<string>> driverData;

	Exlog(AMARELO + __func__, BRANCO + "schemas.size(): " + CIANO + to_string(schemas.size()));

	//para cada schema, adicionar as tabelas dele no html.
	for (string& current_schema : schemas) {
		Exlog(AMARELO + __func__, BRANCO + "current_schema: " + CIANO + current_schema);

		//listar todas as tabelas do schema atual
		vector<string> tables = MySQL::list_tables(current_schema, regex("E[0-9]+_.+")); Exlog(AMARELO + __func__, BRANCO + "tables.size(): " + CIANO + to_string(tables.size()));

		//passar de 1 em 1 as tables do database.
		for (auto& current_table : tables) {
			Exlog(AMARELO + __func__, BRANCO + "current_table: " + CIANO + current_table);

			try {
				con->setSchema(current_schema); Exlog(AMARELO + __func__, BRANCO + "getSchema on: " + CIANO + con->getSchema().asStdString()); Exlog(AMARELO + __func__, BRANCO + "Preparando Statment (pstmt): " + CIANO + QuerySelectAllDrivers(current_table) + " Schema: " + current_schema);

				pstmt = con->prepareStatement(QuerySelectAllDrivers(current_table)); Exlog(AMARELO + __func__, BRANCO + "Realizando SELECT na table " + current_table + " da temporada atual.");

				result = pstmt->executeQuery(); Exlog(AMARELO + __func__, BRANCO + "Query on Schema: " + CIANO + current_schema + BRANCO + " (pstmt): " + CIANO + QuerySelectAllDrivers(current_table));

				string playerId{ "" };

				while (result->next()) {

					// Obtém o valor da coluna playerId
					playerId = result->getString(1);

					if (driverData.find(playerId) == driverData.end()) {

						vector<string> toInsert;

						toInsert.push_back(result->getString(2));
						toInsert.push_back(result->getString(3));
						toInsert.push_back(result->getString(4));
						toInsert.push_back(result->getString(5));
						toInsert.push_back(current_schema);

						driverData[playerId] = toInsert;

					}
					else {
						//Exlog(AMARELO + __func__, AMARELO + "playerId já existe no map, pulando para o próximo.");
					}
				}
			}
			catch (sql::SQLException& e) {
				Exlog(AMARELO + __func__, VERMELHO + "Problema durante o INSERT das analises no MySQL!");
				Exlog(AMARELO + __func__, VERMELHO + "Erro SQL: " + e.what());
				Exlog(AMARELO + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
				Exlog(AMARELO + __func__, VERMELHO + "SQLState: " + e.getSQLState());
			}
		}
	}

	Exlog(AMARELO + __func__, BRANCO + "driverData.size(): " + CIANO + to_string(driverData.size()));

	insertAllDriversOnMySQL(driverData); Exlog(AMARELO + __func__, VERDE + "Realizado insertAllDriversOnMySQL:\n");

	for (const auto& driver : driverData) {
		Exlog(AMARELO + __func__, BRANCO + "driver.first: " + CIANO + driver.first);
		for (const auto& data : driver.second) {
			Exlog(AMARELO + __func__, BRANCO + "data: " + CIANO + data);
		}
	}
	cout << "\n\n";

}