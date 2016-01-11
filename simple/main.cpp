#include <iostream>
#include <qsh/qsh.hpp>

using namespace std;

int main()
{
    qsh::reader::QshReader reader("OrdLog.GAZR-12.15.2015-09-17.qsh");
    cout << reader.app_name() << endl;
    cout << reader.comment() << endl;
    cout << reader.header_datetime() << endl;
    long long i = 0;
    for (const auto& s : reader.streams())
    {
        auto ss = static_cast<qsh::reader::ISecurityStream*>(s);
        cout << ss->security().entry() << endl;
        if (ss->type() == qsh::StreamType::OrdLog)
        {
            auto ols = dynamic_cast<qsh::reader::IOrdLogStream*>(s);
            ols->deal_handler([&i](const qsh::Deal& deal) mutable {
                ++i;
                //cout << deal.datetime << "\n";
            });
        }
    }
    while (!reader.eof())
    {
        reader.read(true);
        //cout << reader.datetime() << "\n";
    }
    cout << "finish: " << i << endl;
    return 0;
}

