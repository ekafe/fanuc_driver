:: set the path to your visual studio vcvars script, it is different for every version of Visual Studio.
set VS2019TOOLS="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"

:: make sure we found them
if not exist %VS2019TOOLS% (
    echo VS 2019 Build Tools are missing!
    exit
)

:: call that script, which essentially sets up the VS Developer Command Prompt
call %VS2019TOOLS%

:: run the compiler with your arguments
cl.exe /EHsc /Zi /Fe: fanuc_driver.exe main.cpp /link .\assets\Fwlib32.lib D:\code_library\mysql_lib\bit32\lib\vs14\mysqlcppconn.lib
exit