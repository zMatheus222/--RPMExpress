#include "mysql_functions.hpp"
#include "AnalysisTools.hpp"

#include <regex>

namespace MySQL {

	sql::Driver* driver = nullptr;
	sql::Connection* con = nullptr;
	sql::Statement* stmt = nullptr;
	sql::ResultSet* result = nullptr;
	sql::PreparedStatement* pstmt = nullptr;

};

using namespace MySQL;
using namespace TXT_Style;
using namespace std;

mutex MySQL::mysql_mutex;

//
//funções padrão mysql
//

MySQL_Auth::MySQL_Auth(string server, string username, string password) {
	mysql_server = server;
	mysql_username = username;
	mysql_password = password;
}

map<string, string> MySQL_Auth::getMySQLAuth() {

	map<string, string> user_data;

	user_data["server"] = mysql_server;
	user_data["username"] = mysql_username;
	user_data["password"] = mysql_password;

	return user_data;

}

void MySQL::Connect(string server, string username, string password) {

	Exlog(AMARELO + __FUNCTION__, StartMsg);

	try {
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		Exlog(AMARELO + __FUNCTION__, VERDE + "Logado com sucesso no banco de dados MySQL!");
	}
	catch (sql::SQLException e) {
		Exlog(AMARELO + __FUNCTION__, VERMELHO + "Não foi possível logar no banco de dados MySQL, Mensagem de erro: " + e.what());
		system("pause");
		exit(1);
	}

	Exlog(AMARELO + __FUNCTION__, EndMsg);
}

vector<string> MySQL::list_schemas(regex filter) {

	lock_guard<mutex> lock(mysql_mutex);

	Exlog(AMARELO + __func__, StartMsg);

	vector<string> schema_list;

	try {

		stmt = con->createStatement();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
		stmt->execute("SHOW DATABASES");

		result = stmt->getResultSet();
		Exlog(AMARELO + __func__, BRANCO + "Schema list:");


		while (result->next()) {

			//listar todos os schemas, menos os de sistema
			if (regex_search("none", filter) && (!regex_match(result->getString(1).asStdString(), regex("information_schema|mysql|performance_schema|sys")))) {
				Exlog(AMARELO + __func__, BRANCO + "getString(1): " + CIANO + result->getString(1).asStdString());
				schema_list.push_back(result->getString(1));
			}

			//listar apenas os schemas filtrados por 'filter'
			if (!regex_search("none", filter) && regex_search(result->getString(1).c_str(),filter)) {
				//Exlog(AMARELO + __func__, BRANCO + "filter" + CIANO + filter + BRANCO + " | getString(1): " + CIANO + result->getString(1).asStdString());
				schema_list.push_back(result->getString(1));
			}

		}

		delete result;
		delete stmt;
	}
	catch (sql::SQLException& e) {

		Exlog(AMARELO + __func__, VERMELHO + "Problema durante a listagem de banco de dados.");
		Exlog(AMARELO + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog(AMARELO + __func__, VERMELHO + "Código do erro: " + to_string(e.getErrorCode()));
		Exlog(AMARELO + __func__, VERMELHO + "SQLState: " + e.getSQLState());

		system("pause");
		exit(1);

	}

	if (schema_list.empty()) {
		schema_list.push_back("none");
	}

	Exlog(AMARELO + __func__, EndMsg);

	return schema_list;

}

vector<string> MySQL::list_tables(string schema, regex filter) {

	lock_guard<mutex> lock(mysql_mutex);

	Exlog(AMARELO + __func__, StartMsg);

	Exlog(AMARELO + __func__, BRANCO + "to_search: " + CIANO + schema);

	vector<string> table_list;

	try {

		con->setSchema(schema);

		Exlog(AMARELO + __func__, BRANCO + "getSchema on: " + CIANO + con->getSchema().asStdString());

		stmt = con->createStatement();

		stmt->execute("SHOW TABLES");
		result = stmt->getResultSet();

		//verificar se existe itens no result, caso sim ele entra no while, senão não, e retorna "none"
		bool have_itens = false;
		while (result->next()) {
			have_itens = true;
			result->beforeFirst();
			break;
		}

		if (!have_itens) {
			table_list.push_back("none");
			return table_list;
		}

		Exlog(AMARELO + __func__, "Tablelist:");
		while (result->next()) {

			//listar todos as tables sem filtro
			if (regex_search("none", filter)) {
				Exlog(AMARELO + __func__, BRANCO + "getString(1): " + CIANO + result->getString(1).asStdString());
				table_list.push_back(result->getString(1));
			}

			//listar apenas as tables com filtro, 'filter'
			if (!regex_search("none", filter) && (regex_search(result->getString(1).asStdString(), filter))) {
				//Exlog(AMARELO + __func__, BRANCO + "filter: " + CIANO + filter + " | " + BRANCO + "getString(1): " + CIANO + result->getString(1).asStdString());
				table_list.push_back(result->getString(1));
			}
			
		}

		Exlog(AMARELO + __func__, VERDE + "Listagem de tabelas concluida.");

	}
	catch (sql::SQLException& e) {

		Exlog(MAGENTA + __func__, VERMELHO + "Algum problema na listagem das tables do schema: " + schema + ", Mensagem de erro : " + e.what());
		Exlog(MAGENTA + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog(MAGENTA + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
		Exlog(MAGENTA + __func__, VERMELHO + "SQLState: " + e.getSQLState());

		system("pause");
		exit(1);

	}

	if (table_list.empty()) {
		table_list.push_back("none");
	}

	Exlog(AMARELO + __func__, EndMsg);

	return table_list;

}

void MySQL::create_schema(string db_name) {

	Exlog(MAGENTA + __func__, StartMsg);

	try {
		stmt = con->createStatement();
		stmt->execute("CREATE DATABASE IF NOT EXISTS " + db_name);
		Exlog(MAGENTA + __func__, BRANCO + "Created Schema: " + CIANO + db_name);
	}
	catch (sql::SQLException& e) {
		
		Exlog(MAGENTA + __func__, VERMELHO + "Não foi possível criar o banco de dados. Mensagem de erro: " + e.what());
		Exlog(MAGENTA + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog(MAGENTA + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
		Exlog(MAGENTA + __func__, VERMELHO + "SQLState: " + e.getSQLState());

		system("pause");
		exit(1);
	}

	delete stmt;

	Exlog(MAGENTA + __func__, EndMsg);
}

void MySQL::create_table(string schema, string table_name, bool drop_if_exists, string table_index) {

	Exlog(MAGENTA + __func__, StartMsg);

	Exlog(MAGENTA + __func__, BRANCO + "table_index: " + CIANO + table_index);

	try {

		con->setSchema(schema);
		Exlog(MAGENTA + __func__, BRANCO + "getSchema on: " + CIANO + con->getSchema().asStdString());

		//criar tabela referente a etapa tal
		stmt = con->createStatement();
		Exlog(MAGENTA + __func__, VERDE + "Created Statement (stmt)");

		if (drop_if_exists == true) {
			stmt->execute("DROP TABLE IF EXISTS " + table_name);
			Exlog(MAGENTA + __func__, VERDE + "Dropped table: " + table_name);
		}

		stmt->execute("CREATE TABLE IF NOT EXISTS " + table_name + " (" + table_index + ");");
		Exlog(MAGENTA + __func__, VERDE + "Created table: " + table_name);

	}
	catch (sql::SQLException& e) {

		Exlog(MAGENTA + __func__, VERMELHO + "Problema durante a criacao da tabela: " + table_name);
		Exlog(MAGENTA + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog(MAGENTA + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
		Exlog(MAGENTA + __func__, VERMELHO + "SQLState: " + e.getSQLState());

		system("pause");
		exit(1);
	}

	delete stmt;

	Exlog(MAGENTA + __func__, EndMsg);

}
/*
void MySQL::update_value(string& schema, string& table, pair<pair<int, string>, string>& values) {

	Exlog(AMARELO + __func__, StartMsg);

	con->setSchema(schema);

	Exlog(AMARELO + __func__, BRANCO + "getSchema on: " + CIANO + con->getSchema().asStdString());

	try {

		pstmt = con->prepareStatement("UPDATE " + table + " SET " + values.first.second + " = ? WHERE id = ?;");

		pstmt->setString(1, values.second);
		pstmt->setInt(2, values.first.first);

		pstmt->executeUpdate();


		delete result;
		delete stmt;
		delete pstmt;
	
	
	}
	catch (sql::SQLException& e) {

		Exlog(MAGENTA + __func__, VERMELHO + "Problema durante o INSERT das analises no MySQL!");
		Exlog(MAGENTA + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog(MAGENTA + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
		Exlog(MAGENTA + __func__, VERMELHO + "SQLState: " + e.getSQLState());

		system("pause");
		exit(1);

	}	

	Exlog(AMARELO + __func__, EndMsg);
}
*/

int MySQL::findDriverOnTable(string& needed_table, string& piloto_acusado) {

	Exlog(AMARELO + __func__, StartMsg);

	int playerKeyId{ 0 };

	con->setSchema(DefaultSeason);

	Exlog(AMARELO + __func__, BRANCO + "Realizando setSchema em: " + CIANO + con->getSchema().asStdString());

	try {

		//se o item passado foi um steam guid
		if (regex_match(piloto_acusado, regex("^S\\d{10,25}$"))) {

			//buscar playerId com base no 'lastName' nome do piloto acusado na tabela da etapa
			pstmt = con->prepareStatement(QueryFindDriverOnTableSTEAMGUID(needed_table, piloto_acusado));

			Exlog(AMARELO + __func__, BRANCO + "SteamGuid::Preparando Statment (pstmt): " + CIANO + QueryFindDriverOnTableSTEAMGUID(needed_table, piloto_acusado));

			result = pstmt->executeQuery();

			Exlog(AMARELO + __func__, BRANCO + "Realizando SELECT na table " + needed_table + " da temporada atual.");

			while (result->next()) {
				playerKeyId = result->getInt(1);
				Exlog(AMARELO + __func__, BRANCO + "PlayerId: " + CIANO + to_string(playerKeyId));
			}

			Exlog(AMARELO + __func__, VERDE + "Realizado Statement (pstmt).");

		}
		else {

			//buscar playerId com base no 'lastName' nome do piloto acusado na tabela da etapa
			pstmt = con->prepareStatement(QueryFindDriverOnTable(needed_table, piloto_acusado));

			Exlog(AMARELO + __func__, BRANCO + "lastName::Preparando Statment (pstmt): " + CIANO + QueryFindDriverOnTable(needed_table, piloto_acusado));

			result = pstmt->executeQuery();

			Exlog(AMARELO + __func__, BRANCO + "Realizando SELECT na table " + needed_table + " da temporada atual.");

			while (result->next()) {
				playerKeyId = result->getInt(1);
				Exlog(AMARELO + __func__, BRANCO + "PlayerId: " + CIANO + to_string(playerKeyId));
			}

			Exlog(AMARELO + __func__, VERDE + "Realizado Statement (pstmt).");
		}




	}
	catch (sql::SQLException& e) {
		Exlog(AMARELO + __func__, VERMELHO + "Problema durante o INSERT das analises no MySQL!");
		Exlog(AMARELO + __func__, VERMELHO + "Erro SQL: " + e.what());
		Exlog(AMARELO + __func__, VERMELHO + "Código de erro: " + to_string(e.getErrorCode()));
		Exlog(AMARELO + __func__, VERMELHO + "SQLState: " + e.getSQLState());
	}

	delete pstmt;
	delete result;

	return playerKeyId;
}