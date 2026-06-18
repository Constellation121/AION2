@echo off
pushd %~dp0

echo ==========================================
echo           Start Generating Packets        
echo ==========================================

:: 1. Protobuf 컴파일
echo [1/2] Compiling proto files...
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

:: 컴파일 중 에러가 발생하면 복사하지 않고 중
IF ERRORLEVEL 1 (
    echo [Error] Protobuf Compilation Failed!
    PAUSE
    EXIT /B
)

 for %%f in (*.pb.h) do (
     echo #ifdef verify >> temp.h
     echo #undef verify >> temp.h
     echo #endif >> temp.h
     echo #ifdef check >> temp.h
     echo #undef check >> temp.h
     echo #endif >> temp.h
     type %%f >> temp.h
     move /y temp.h %%f
 )

:: 2. 파일 복사
echo [2/2] Copying files to AION2Server...

set "SERVER_PATH=../AION2Server/AION2Server"

XCOPY /Y Enum.pb.h "%SERVER_PATH%\"
XCOPY /Y Enum.pb.cc "%SERVER_PATH%\"
XCOPY /Y Struct.pb.h "%SERVER_PATH%\"
XCOPY /Y Struct.pb.cc "%SERVER_PATH%\"
XCOPY /Y Protocol.pb.h "%SERVER_PATH%\"
XCOPY /Y Protocol.pb.cc "%SERVER_PATH%\"

set "CLIENT_PATH=../Source/AION2/Network"

XCOPY /Y Enum.pb.h "%CLIENT_PATH%\"
XCOPY /Y Enum.pb.cc "%CLIENT_PATH%\"
XCOPY /Y Struct.pb.h "%CLIENT_PATH%\"
XCOPY /Y Struct.pb.cc "%CLIENT_PATH%\"
XCOPY /Y Protocol.pb.h "%CLIENT_PATH%\"
XCOPY /Y Protocol.pb.cc "%CLIENT_PATH%\"

:: 3. 임시 파일 제거
echo Cleaning up temporary files...
DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc


echo ==========================================
echo          Generation Successfully Done     
echo ==========================================
PAUSE