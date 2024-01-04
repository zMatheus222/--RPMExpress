#pragma once

#include "default_functions.hpp"

#include <map>
#include <utility>
#include <regex>

//bibliotecas MySQL
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

namespace MySQL {

	std::vector<std::string> list_schemas(std::regex filter);
	std::vector<std::string> list_tables(std::string schema, std::regex filter);
	void create_schema(std::string db_name);
	void create_table(std::string schema, std::string table_name, bool drop_if_exists, std::string table_values);
	void Connect(std::string server, std::string username, std::string password);

	//void update_value(std::string& schema, std::string& table, std::pair<std::pair<int, std::string>, std::string>& values);
	
	int findDriverOnTable(std::string& needed_table, std::string& piloto_acusado);

	std::string analysisRequest_html_fill(const char* START);
	std::string analysis_html_fill(const std::map<std::string, std::string>& AnalysisBase, const char* START);
	std::string results_html_fill(std::vector<std::string>& schemas, const char* START);
	void insert_on_analysis_table(std::map<std::string, std::string>& post_data);

	void insert_on_results_table(std::map<std::string, std::string>& tableData);
	bool verify_existing_content(std::string& schema, std::string& table);

	void insertOnStageTable(std::pair<int, int> etapa_bateria, std::string piloto_acusado, int Punicao_Pontos, int Punicao_Tempo, std::string Punicoes_Adicionais);
	void update_geral_ranking();

	extern sql::Driver* driver;
	extern sql::Connection* con;
	extern sql::Statement* stmt;
	extern sql::ResultSet* result;
	extern sql::PreparedStatement* pstmt;

	extern std::mutex mysql_mutex;

};

class MySQL_Auth {

private:
	std::string mysql_server{ "" };
	std::string mysql_username{ "" };
	std::string mysql_password{ "" };
public:
	MySQL_Auth(std::string server, std::string username, std::string password);
	std::map<std::string, std::string> getMySQLAuth();
};