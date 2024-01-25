#pragma once

#include "default_functions.hpp"
#include "mysql_functions.hpp"

namespace Analysis_Tools {

	void AnalysisRoutes();
	
	void update_analysis_show();

	void operator_menu_main();

	void update_analysisRequest_show();

	std::map<std::string, std::string> PickPunishmentBySituation(std::string& piloto_punido, std::string& situacaoPrincipal, std::string& situacaoIntrinseca, bool reincidencia);
	std::map<std::string, std::string> PickPunishmentBySituation(json& data);

};