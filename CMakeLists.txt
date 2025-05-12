cmake_minimum_required (VERSION 3.30)
project (jacl VERSION 25.0.1 DESCRIPTION "Just Another Compiled Language" LANGUAGES CXX)

add_executable (${PROJECT_NAME}
	"src/main.cpp"
	"src/lexer.cpp"
	"src/token.cpp"
	
	"src/parse/program.cpp"
	"src/parse/parser.cpp"
	"src/parse/statement.cpp"
	"src/parse/qualified_type.cpp"
	"src/parse/scope.cpp"
	"src/parse/func_def.cpp"
	"src/parse/return.cpp"
	"src/parse/literal.cpp"

	"src/gen/generator.cpp"
	"src/gen/fun_def.cpp"
	"src/gen/program.cpp"
	"src/gen/literal.cpp"
	"src/gen/scope.cpp"
	"src/gen/statement.cpp"
	"src/gen/return.cpp"
)

target_compile_options (${PROJECT_NAME} PRIVATE -g -Wall -Wextra -Wpedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic)
target_include_directories (${PROJECT_NAME} PRIVATE "include")

set_target_properties (${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED TRUE
    VERSION ${PROJECT_VERSION}
	EXPORT_COMPILE_COMMANDS TRUE
)

