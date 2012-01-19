/***************************************************************************
*   @author Matías Hernández Arellano <mhernandez@visionlabs.cl>
*   @author VisionLabs S.A
*   @version 0.1
*
*   This file is part of Contador.
*
*    Contador is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*   Contador is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Contador.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/


/*************************************************************************************
*  Este archivo presenta la implementación de la clase "Database". Permite la conección,
*  Y posterior inserción de datos en una base de datos MySQL
*
*****************************************************************************/
#include "database.h"
#include <QTimer>

Database* Database::instancia = NULL; //Singleton

bool Database::setConnection(const char* db,const  char* server,const  char* user,const char* pass){
        database = db;
        host = server;
        usuario = user;
        passwd = pass;
        if(!mysql_init(&mysql)){
            return false;
        }
        mysql_options(&mysql,MYSQL_OPT_RECONNECT,"true");
        connection = mysql_real_connect(&mysql,server,user,pass,db,MYSQL_PORT,NULL,0);
        mysql_options(&mysql,MYSQL_OPT_RECONNECT,"true");
        if(connection==NULL){
            mysql_close(&mysql);
            return false;
        }
        if(mysql_select_db(&mysql,db)){
            qDebug()<<"Error Conectando: "<< mysql_error(&mysql);
            mysql_close(&mysql);
            return false;
        }
        qDebug()<<"Database Connected";
        return true;
}

bool Database::setConnection(){
        mysql_close(&mysql);

        if(!mysql_init(&mysql)){
            return false;
        }
        mysql_options(&mysql,MYSQL_OPT_RECONNECT,"true");
        connection = mysql_real_connect(&mysql,host,usuario,passwd,database,MYSQL_PORT,NULL,0);
        mysql_options(&mysql,MYSQL_OPT_RECONNECT,"true");
        if(connection==NULL){
            mysql_close(&mysql);
            return false;
        }
        if(mysql_select_db(&mysql,database)){
            qDebug()<<"Error Conectando 2: "<<mysql_error(&mysql);
            mysql_close(&mysql);
            return false;
        }
        return true;
}


Database::Database(){

}

Database::~Database(){

    mysql_close(&mysql);
}

bool Database::isConnected(){
    //if(mysql.status){
    //    return false;
    //}
    if(mysql_ping(&mysql))
        return true;
    return false;
}


bool Database::insertar(QString sql){

    std::string str= std::string(sql.toAscii().data());
    if(mysql_query(&mysql,str.c_str())){
        qDebug()<<"Error Insertar: "<<mysql_error(&mysql);
        return false;
    }else{
        qDebug()<<"Valores insertado";
        return true;
    }
}

bool Database::query(QString sql){
    return insertar(sql);
}

int Database::totalRegistros(){
    if(!mysql_ping(&mysql)){
        setConnection();
    }
    int total=0;

    if(mysql_query(&mysql,"SELECT * FROM registro")){
        qDebug()<<"Error Total: "<<mysql_error(&mysql);
        if(!isConnected()){
            setConnection();
        }
        return false;
    }else{
        MYSQL_RES *res;
        if((res = mysql_store_result(&mysql))){
            total = mysql_num_rows(res);
        }
        mysql_free_result(res);
    }
    return total;
}


void Database::closeConnection(){
    mysql_close(&mysql);
}

/*void Database::bloque(XnUInt16 nUsers,time tiempos[]){

}*/
