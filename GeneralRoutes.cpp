/*
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
	crow::SimpleApp Route;

	// n�vel de log para DEBUG (ou outro n�vel desejado)
	crow::logger::setLogLevel(crow::LogLevel::Debug);


	//executar todas as rotas crow
	Route.port(8085).run();


}
*/