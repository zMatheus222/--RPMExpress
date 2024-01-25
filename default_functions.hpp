#pragma once

//bibliotecas padrão C++
#include <array>
#include <string>

#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace TXT_Style {

	constexpr std::string PRETO = "";
	constexpr std::string VERMELHO = "";
	constexpr std::string VERDE = "";
	constexpr std::string AMARELO = "";
	constexpr std::string AZUL = "";
	constexpr std::string MAGENTA = "";
	constexpr std::string CIANO = "";
	constexpr std::string BRANCO = "";
	constexpr std::string ENDS = "";

	/*
	constexpr std::string PRETO = "\033[30m";
	constexpr std::string VERMELHO = "\033[31m";
	constexpr std::string VERDE = "\033[32m";
	constexpr std::string AMARELO = "\033[33m";
	constexpr std::string AZUL = "\033[34m";
	constexpr std::string MAGENTA = "\033[35m";
	constexpr std::string CIANO = "\033[36m";
	constexpr std::string BRANCO = "\033[37m";
	constexpr std::string ENDS = "\033[0m\n";
	*/

	constexpr std::string PADRAO = "\033[0m";
	constexpr std::string NEGRITO = "\033[1m";
	constexpr std::string FRACO = "\033[2m";
	constexpr std::string ITALICO = "\033[3m";
	constexpr std::string SUBLINHADO = "\033[4m";
	constexpr std::string INVERTE_CORES = "\033[7m";

}

//classe em modelo Singleton

class Directories {

private:
	Directories() {}; // O construtor Directories é privado, o que significa que não pode ser chamado diretamente por código externo. Isso impede a criação de instâncias da classe fora do seu próprio escopo.
	~Directories() {}; // O destrutor Directories também é privado. Isso impede que instâncias da classe sejam destruídas fora do seu próprio escopo. Nesse caso, se a classe for uma implementação de Singleton, pode ser que não desejemos destruir a instância durante a execução do programa.

	// Evita cópia e atribuição ao usar Directories(const Directories&) = delete; e Directories& operator=(const Directories&) = delete;, você está indicando explicitamente que a cópia e a atribuição são operações proibidas.
	Directories(const Directories&) = delete;
	Directories& operator=(const Directories&) = delete;

	// Variáveis da struct

	// ... outras variáveis ...

public:
	static Directories& getInstance() {
		static Directories instance; // Será criada apenas uma vez
		return instance;
	}

	//função que inicializa as variaveis
	void Initialize();

	static bool AnalysisRoutesOnEnd;
	static bool RankingToolsOnEnd;
	static bool DiscordToolsRoutesOnEnd;
	static bool RPMDownloaderOnEnd;

	static bool RunningAnalysisUpdate;
	static bool RunningRankingTools;

	//static bool RPMDownloaderOnEnd;

	// Outras funções e variáveis públicas, se necessário...
	static std::string base_dir;
	static std::string templates_dir;
	static std::string static_dir;
	static std::string const_dir;
	static std::string results_dir;

	static std::string operator_auth_dir;
	static std::string championship_configs_dir;

	static std::string analysis_request_dir;
	static std::string analysis_request_after_dir;
	static std::string analysis_show_dir;
	static std::string analysis_show_after_dir;
	static std::string operator_menu_dir;
	static std::string table_dir;
	static std::string table_after_dir;

	static json operator_auth;
	static json championship_configs;

	//importação geral de arquivos
	static std::pair<std::string, json> import_file(const std::string& file_path);

	//importação de arquivos com wstring (utf 16 e etc)
	static std::wstring import_wfile(const std::string& file_path);

	//função de busca no MySQL
	static std::map<std::string, std::string> search_in_rpm_base(std::string needed_schema, std::string needed_table, std::string table_query);

	//lista os arquivos de um diretório:
	static std::vector<std::string> list_files(const char* path, bool ignore_folders);

	//listar e criar tabela com todos os pilotos
	static void registerAllDrivers();
};

#define Debug_Logs Directories::operator_auth["Debug_Logs"]
#define Exlog_Shows Directories::operator_auth["Exlog_Shows"]
#define Exlog_Colors Directories::operator_auth["Exlog_Colors"]

#define getstr get<string>()

#define MySQL_Server   Directories::operator_auth["MySQL_Keys"]["MySQL_Server"]
#define MySQL_Username Directories::operator_auth["MySQL_Keys"]["MySQL_Username"]
#define MySQL_Password Directories::operator_auth["MySQL_Keys"]["MySQL_Password"]

#define StartMsg VERDE + "Started!"
#define EndMsg AMARELO + "Ended!"
#define AnalysisDefaultTable Directories::championship_configs["ChampMain"]["AnalysisTable"]
#define DefaultSeason Directories::championship_configs["ChampMain"]["Season"].get<string>()

#define DiscordBotToken Directories::operator_auth["Discord_Keys"]["BOT_TOKEN"]
#define ChannelAnalysisAlert Directories::operator_auth["Discord_Keys"]["AA_Alert_ChannelID"]
#define ChannelAnalysisShow Directories::operator_auth["Discord_Keys"]["AA_Show_ChannelID"]

#define RegexSeason "T\\d{1,3}"

#define QueryAnalysisValues "id serial PRIMARY KEY, DataDoPedido VARCHAR(30), Piloto_Solicitante VARCHAR(30), Piloto_Acusado VARCHAR(30), Simulador VARCHAR(30), Etapa INT, Bateria INT, Minuto VARCHAR(30), Gravacao_Replay VARCHAR(200), Contexto TEXT, Situacao_Principal VARCHAR(200), Situacao_Intrinseca VARCHAR(200), Punicao_Tempo INT, Punicao_Pontos INT, Punicoes_Adicionais VARCHAR(50)"
#define QueryAnalysisInsert "INSERT INTO " + Directories::championship_configs["ChampMain"]["AnalysisTable"].get<string>() + " (DataDoPedido, Piloto_Solicitante, Piloto_Acusado, Simulador, Etapa, Bateria, Minuto, Gravacao_Replay, Contexto) VALUES(?,?,?,?,?,?,?,?,?)"
#define QueryPunishmentInsert "UPDATE " + Directories::championship_configs["ChampMain"]["AnalysisTable"].get<string>() + " SET Situacao_Principal = ?, Situacao_Intrinseca = ?, Punicao_Tempo = ?, Punicao_Pontos = ?, Punicoes_Adicionais = ? WHERE id = ?"
#define QueryStageUpdate(table) "UPDATE " + table + " SET Punicao_Tempo = ?, Punicao_Pontos = ?, Punicoes_Adicionais = ? WHERE id = ?"

//query para verificar se o piloto existe na tabela de pilotos com base no lastName
#define QueryFindDriverOnTable(needed_table, piloto_acusado) "SELECT id FROM " + needed_table + " WHERE lastName = '" + piloto_acusado + "'"

//query para verificar se o piloto existe na tabela de pilotos com base no playerId
#define QueryFindDriverOnTableSTEAMGUID(needed_table, piloto_acusado) "SELECT id FROM " + needed_table + " WHERE playerId = '" + piloto_acusado + "'"

#define SchemaAllDrivers "RPM_Drivers"
#define TableAllDrivers "All_Drivers"
#define QuerySelectAllDrivers(table) "SELECT playerId, shortName, firstName, lastName, teamName FROM " + table
#define QueryInsertAllDrivers(table) "INSERT INTO " + table + " (playerId, shortName, firstName, lastName, teamName, lastSeason) SELECT ?,?,?,?,?,? WHERE NOT EXISTS(SELECT 1 FROM " + table + " WHERE playerId = ? AND shortName = ? AND firstName = ? AND lastName = ? AND teamName = ? AND lastSeason = ?)"

#define QueryAllDriversValues "id serial PRIMARY KEY, playerId VARCHAR(30), shortName VARCHAR(4), firstName VARCHAR(25), lastName VARCHAR(25), teamName VARCHAR(35), lastSeason VARCHAR(3)"

#define QueryResultValues "id serial PRIMARY KEY, playerId VARCHAR(30), cupCategory INT, raceNumber INT, shortName VARCHAR(4), firstName VARCHAR(30), lastName VARCHAR(30), teamName VARCHAR(30), carModel VARCHAR(25), lapCount INT, bestLap INT, totalTime INT, Punicao_Tempo INT, Punicao_Pontos INT, Punicoes_Adicionais VARCHAR(50), Pontos INT"
#define QueryResultInsert(table) "INSERT INTO " + table + " (playerId, cupCategory, raceNumber, shortName, firstName, lastName, teamName, carModel, lapCount, bestLap, totalTime, Pontos) VALUES(?,?,?,?,?,?,?,?,?,?,?,?)"

#define TabelaGeralIndex "playerId VARCHAR(30) PRIMARY KEY, cupCategory INT, raceNumber INT, shortName VARCHAR(4), firstName VARCHAR(30), lastName VARCHAR(30), teamName VARCHAR(30), carModel VARCHAR(25), lapCount INT, bestLap INT, totalTime INT, Punicao_Tempo INT, Punicao_Pontos INT, Punicoes_Adicionais VARCHAR(50), E1B1 INT, E1B2 INT, E2B1 INT, E2B2 INT, E3B1 INT, E3B2 INT, E4B1 INT, E4B2 INT, E5B1 INT, E5B2 INT, TotalPontos INT"

#define TableGeralInsert(tableData, etapa_bateria) R"(
INSERT INTO Tabela_Geral (
    playerId, 
    cupCategory, 
    raceNumber, 
    shortName, 
    firstName, 
    lastName, 
    teamName, 
    carModel, 
    lapCount, 
    bestLap, 
    totalTime,
    Punicao_Tempo,
    Punicao_Pontos,
    Punicoes_Adicionais,
    )" + etapa_bateria + R"(
)
VALUES (
    COALESCE()" + tableData["playerId"] + R"(, NULL),
    COALESCE()" + tableData["cupCategory"] + R"(, 0),
    COALESCE()" + tableData["raceNumber"] + R"(, 0),
    COALESCE()" + tableData["shortName"] + R"(, NULL),
    COALESCE()" + tableData["firstName"] + R"(, NULL),
    COALESCE()" + tableData["lastName"] + R"(, NULL),
    COALESCE()" + tableData["teamName"] + R"(, NULL),
    COALESCE()" + tableData["carModel"] + R"(, NULL),
    COALESCE()" + tableData["lapCount"] + R"(, 0),
    COALESCE()" + tableData["bestLap"] + R"(, 0),
    COALESCE()" + tableData["totalTime"] + R"(, 0),
    COALESCE()" + tableData["Punicao_Tempo"] + R"(, 0),
    COALESCE()" + tableData["Punicao_Pontos"] + R"(, 0),
    COALESCE()" + tableData["Punicoes_Adicionais"] + R"(, NULL),
    COALESCE()" + tableData[etapa_bateria] + R"(, 0)
)
ON DUPLICATE KEY UPDATE
    cupCategory = VALUES(cupCategory),
    raceNumber = VALUES(raceNumber),
    shortName = VALUES(shortName),
    firstName = VALUES(firstName),
    lastName = VALUES(lastName),
    teamName = VALUES(teamName),
    carModel = VALUES(carModel),
	lapCount = IF(lapCount IS NULL OR lapCount = '', VALUES(lapCount), lapCount),
	bestLap = IF(bestLap IS NULL OR bestLap = '', VALUES(bestLap), bestLap),
    totalTime = IF(totalTime IS NULL OR totalTime = '', VALUES(totalTime), totalTime),
    Punicao_Tempo = IF(Punicao_Tempo IS NULL OR Punicao_Tempo = '', VALUES(Punicao_Tempo), Punicao_Tempo),
    Punicao_Pontos = IF(Punicao_Pontos IS NULL OR Punicao_Pontos = '', VALUES(Punicao_Pontos), Punicao_Pontos),
    Punicoes_Adicionais = IF(Punicoes_Adicionais IS NULL OR Punicoes_Adicionais = '', VALUES(Punicoes_Adicionais), Punicoes_Adicionais),
	)" + etapa_bateria + R"( = IF()" + etapa_bateria + R"( IS NULL OR )" + etapa_bateria + R"( = '', VALUES()" + etapa_bateria + R"(), )" + etapa_bateria + R"(
);
)"

#define TableGeralSelect(schema) R"(
	SELECT COALESCE(cupCategory, 0) as Classe,
	COALESCE(raceNumber, 0) as Num,
	COALESCE(shortName, "PLA") as Tag,
	COALESCE(lastName, "none") as Nome,
	COALESCE(teamName, "none") as Equipe,
	COALESCE(carModel, "none") as Modelo,
	COALESCE(Punicao_Tempo, 0) as Pun_Tempo,
	COALESCE(Punicao_Pontos, 0) as Pun_Pontos,
	COALESCE(Punicoes_Adicionais, 0) as Pun_Adicionais,
	COALESCE(E1B1, 0) as E1B1,
	COALESCE(E1B2, 0) as E1B2,
	COALESCE(E2B1, 0) as E2B1,
	COALESCE(E2B2, 0) as E2B2,
	COALESCE(E3B1, 0) as E3B1,
	COALESCE(E3B2, 0) as E3B2,
	COALESCE(E4B1, 0) as E4B1,
	COALESCE(E4B2, 0) as E4B2,
	COALESCE(E5B1, 0) as E5B1,
	COALESCE(E5B2, 0) as E5B2,
	COALESCE(TotalPontos, 0) as Pontos
	FROM )" + schema + R"(.Tabela_Geral;
)"

#define ConfigBaseQuery(column, fileType) "SELECT " + column + " FROM RPMDownloader WHERE file_type = '" + string(fileType) + "'"

//void export_file(std::string* file_data, const std::string& file_name);

std::string validate_link(std::string& link);

void Exlog(std::string function, std::string message);
void Exlog(const char* function, const char* service, const char* route_type, const char* message);
void Exlog(const char* function, const char* service, const char* route_type, std::string message);

void rpm_downloader();
extern std::string DownloaderBase;