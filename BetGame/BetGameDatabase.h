#pragma once
#include <vector>

#include "third_party/chromium/sql/connection.h"


struct BetResult;

class BetGameDatabase
{
public:
    class SQLErrorLog : public sql::ErrorDelegate
    {
    public:
        SQLErrorLog() : errCounter_(0)
        {
        }

        virtual int OnError(int error, sql::Connection* connection, sql::Statement* stmt);

        inline void reset()
        {
            errCounter_ = 0;
        }

    protected:
        int errCounter_;
    };

    BetGameDatabase();
    ~BetGameDatabase();

    // �����ݿ⣬����������򴴽����
    bool Initialize(const std::wstring& file_name);

    // ������flush�����ݿⲢ�ҹر����ݿ⼰�ļ�
    void Finalize();

    // ִ�в������ݲ���
    bool InsertRecord(const BetResult& bet_result);

    // ��ѯ���е�ԭʼ��¼����
    bool Query(std::vector<BetResult>* bet_results);
    
private:
    sql::Connection db_conn_;
    SQLErrorLog* sqlLog_;

    bool OpenDatabase(const std::wstring& file_name);
    bool IntegrityCheck();
};

