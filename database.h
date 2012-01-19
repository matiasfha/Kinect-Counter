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
#ifndef DATABASE_H
#define DATABASE_H
#include <XnOS.h>
#include <QDebug>
#include <QString>
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
    #include <mysql5/mysql/mysql.h>
    #include <mysql5/mysql/mysqld_error.h>
#else
    #include <mysql/mysql.h>
    #include <mysql/mysqld_error.h>
#endif


#define DB    Database::getInstancia() //Alias para obtener la instancia de la base de datos

class Database: public QObject
{
    Q_OBJECT
public:
    /* Singleton */
    static Database* getInstancia(){
        if(!instancia)
            instancia = new Database;
        return instancia;
    }
    ~Database();
    bool setConnection(const char* db,const char* server,const char* user,const char* pass);
    bool setConnection();
    bool isConnected();
    int  totalRegistros();
    bool insertar(QString);
    bool query(QString);
    void closeConnection();

private:
    MYSQL mysql,*connection;
    const char * database;
    const char * host;
    const char * usuario;
    const char * passwd;
    Database();
    //Singleton
    static Database* instancia;

};

class CGenerateIDs
{
  private:
    char* m_pstrTags;
    int m_nRange;

  public:
    CGenerateIDs (int range = 1000)
    {
      m_nRange = range;
      m_pstrTags = new char [range+1];
      if (m_pstrTags) {
        memset (m_pstrTags, ' ', range);
        m_pstrTags [range] = '\0';
      }
    }
    ~CGenerateIDs (){
        delete [] m_pstrTags;
    }

    int New (){
        if (! m_pstrTags) {
          return -1; // No memory - NO IDs for you!
        }

        char* pstrEmptySlotAt = strchr (m_pstrTags, ' ');

        if (! pstrEmptySlotAt) {
          return -1; // All ID slots are occupied
        }
        pstrEmptySlotAt [0] = '*';
        return (int) (pstrEmptySlotAt - m_pstrTags);
      }

    void Release (int ID){
        if (m_pstrTags) {
          if ((ID >= 0) && (ID < m_nRange)) {
            m_pstrTags [ID] = ' ';
          }
        }
      }

};

#endif // DATABASE_H
