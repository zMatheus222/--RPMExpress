#include "mysql_functions.hpp"

using namespace std;
using namespace TXT_Style;
using namespace MySQL;

const char* MySQL::PickLink(const char* data, string dataColumn) {

    try {
        string link{ "" };

        con->setSchema("ConfigBase");
        stmt = con->createStatement();
        result = stmt->executeQuery(ConfigBaseQuery(dataColumn, data));

        if (result->next()) {
            link = result->getString(dataColumn);
            Exlog(AZUL + __func__, VERDE + "getString: " + link);
        }
        else {
            cout << "Nenhum resultado encontrado." << endl;
        }

        const char* link_c = link.c_str();

        delete stmt;
        delete result;

        return link_c;
    }
    catch (sql::SQLException& e) {
        Exlog(VERMELHO + __func__, VERMELHO + e.what());
        //Exlog(VERMELHO + __func__, VERMELHO + e.getErrorCode());
        Exlog(VERMELHO + __func__, VERMELHO + e.getSQLState());
	}   

    return "ERROR::<><>";

}