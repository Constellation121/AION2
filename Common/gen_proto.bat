@echo off
pushd %~dp0

echo ==========================================
echo           Start Generating Packets        
echo ==========================================

:: 1. Protobuf 컴파일
echo [1/2] Compiling proto files...
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./GameProtocol.proto

:: 컴파일 중 에러가 발생하면 복사하지 않고 중
IF ERRORLEVEL 1 (
    echo [Error] Protobuf Compilation Failed!
    PAUSE
    EXIT /B
)s

:: 2. 파일 복사
echo [2/2] Copying files to AION2Server...

set "SERVER_PATH=../AION2Server/AION2Server"

XCOPY /Y Enum.pb.h "%SERVER_PATH%"
XCOPY /Y Enum.pb.cc "%SERVER_PATH%"
XCOPY /Y Struct.pb.h "%SERVER_PATH%"
XCOPY /Y Struct.pb.cc "%SERVER_PATH%"
XCOPY /Y GameProtocol.pb.h "%SERVER_PATH%"
XCOPY /Y GameProtocol.pb.cc "%SERVER_PATH%"

:: 3. 임시 파일 제거
echo Cleaning up temporary files...
DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc

echo ==========================================
echo          Generation Successfully Done     
echo ==========================================
PAUSE