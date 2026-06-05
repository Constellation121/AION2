@echo off
set SRC_DIR=.
set DST_DIR=.

:: vcpkg에 설치된 protoc와 gRPC 플러그인 경로 설정
set PROTOC=C:\vcpkg\installed\x64-windows\tools\protobuf\protoc.exe
set GRPC_PLUGIN=C:\vcpkg\installed\x64-windows\tools\grpc\grpc_cpp_plugin.exe

echo Generating gRPC and Protocol Buffers codes...

:: 1. 일반 Message (직렬화 코드가 담긴 pb.h, pb.cc 생성)
%PROTOC% -I=%SRC_DIR% --cpp_out=%DST_DIR% %SRC_DIR%\*.proto

:: 2. gRPC Service (네트워크 파이프라인 코드가 담긴 grpc.pb.h, grpc.pb.cc 생성)
%PROTOC% -I=%SRC_DIR% --grpc_out=%DST_DIR% --plugin=protoc-gen-grpc=%GRPC_PLUGIN% %SRC_DIR%\*.proto

echo Done!
pause