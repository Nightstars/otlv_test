// otlv_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


// testotlv4.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
using namespace std;

#include <stdio.h>
#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
#include "otlv4.h" // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{
    otl_stream o(10, // buffer size
        "insert into test_tab values(:f1<int>,:f2<char[31]>)",
        // SQL statement
        db // connect object
    );

    o.set_commit(0); // set stream's auto-commit to OFF.

    try {
        o << 1 << "Line1"; // Enter one row into the stream
        o.flush(); // flush the strem buffer, i.e. force
                   // the stream to execute
#if defined(OTL_ANSI_CPP_11_VARIADIC_TEMPLATES)
        otl_write_row(o, 1, "Line1");
#else
                   // when variadic template functions are not supported by the C++
                   // compiler, OTL provides nonvariadic versions of the same template
                   // functions in the range of [1..15] parameters
        otl_write_row(o, 1, "Line1");


        // the old way (operators >>() / <<()) is available as always:
        // o<<1<<"Line1"; // Enter the same data into the stream
        // and cause a "duplicate key" error.
#endif
        o.flush();
    }
    catch (otl_exception& p) {
        if (p.code == 2601) {
            // ... duplicate key ...
            cout << "STREAM ERROR STATE=" << o.get_error_state() << endl;
            o.clean(1); // clean up the stream's buffer
                        // and clean up the stream's internal
                        // error flag as well. By doing this,
                        // it's possible to recover from
                        // a database error without closing
                        // the stream. Remember, the number of
                        // situtation when it's possible is
                        // limited and the recovery procedure should
                        // be carefully designed.
        }
        else
            throw; // re-throw the exception to the outer catch block.
    }


    o << 2 << "Line2"; // Enter one more row of data after
                       // recovering from the "duplicate key"
                       // error
    o.flush();

    db.commit(); // commit transaction

}

void select()
{
    otl_stream i(10, // buffer size
        "select * from test_tab",
        // SELECT statement
        db // connect object
    );
    // create select stream

    int f1;
    char f2[31];

#if (defined(_MSC_VER) && _MSC_VER>=1600) || defined(OTL_CPP_11_ON)
    // C++11 (or higher) compiler
    for (auto& it : i) {
#if defined(OTL_ANSI_CPP_11_VARIADIC_TEMPLATES)
        otl_read_row(it, f1, f2);
#else
        // when variadic template functions are not supported by the C++
        // compiler, OTL provides nonvariadic versions of the same template
        // functions in the range of [1..15] parameters
        otl_read_row(it, f1, f2);

        // the old way (operators >>() / <<()) is available as always:
        // it>>f1>>f2;
#endif
        cout << "f1=" << f1 << ", f2=" << f2 << endl;
    }
#else
    // C++98/03 compiler
    while (!i.eof()) { // while not end-of-data
        i >> f1 >> f2;
        cout << "f1=" << f1 << ", f2=" << f2 << endl;
    }
#endif

}

int main()
{
    otl_connect::otl_initialize(); // initialize ODBC environment
    try {

        //  CString strCon;
        //strCon.Format("driver=sql server;server=%s;UID=%s;PWD=%s;database=%s",
        //  strServerIP.c_str(), strServerUID.c_str(), strServerPWD.c_str(), strServerDatabase.c_str());
        string strCon1 = "driver=sql server;server=192.168.0.187,60000\SECOP;UID=sa;PWD=Secop_unknow;database=CMS";
        //strCon1 = "driver=sql server;server=127.0.0.1;UID=sa;PWD=12345678;database=aa";
        db.rlogon(strCon1.c_str()); // connect to the database

        otl_cursor::direct_exec
        (
            db,
            "drop table test_tab",
            otl_exception::disabled // disable OTL exceptions
        ); // drop table

        otl_cursor::direct_exec
        (
            db,
            "create table test_tab(f1 int, f2 varchar(30))"
        );  // create table

        //otl_cursor::direct_exec
        //(
        //    db,
        //    "create unique index ind001 on test_tab(f1)"
        //);  // create unique index

        insert(); // insert records into table
        select(); // select records from table

    }

    catch (otl_exception& p) { // intercept OTL exceptions
        cout << p.code << endl; // print out error code
        cout << p.sqlstate << endl; // print out error SQLSTATE
        cout << p.msg << endl; // print out error message
        cout << p.stm_text << endl; // print out SQL that caused the error
        cout << p.var_info << endl; // print out the variable that caused the error
    }

    db.logoff(); // disconnect from the database

    return 0;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
