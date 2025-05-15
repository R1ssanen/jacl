cmake_minimum_required (VERSION 4.0)
project (jacl VERSION 25.0.2 DESCRIPTION "Just Another Compiled Language" LANGUAGES CXX)

add_executable (${PROJECT_NAME}
	"src/main.cpp"
	"src/lexer.cpp"
	"src/token.cpp"
	"src/parser.cpp"
	"src/generator.cpp"

	"src/lang/program.cpp"
	"src/lang/statement.cpp"
	"src/lang/qualified_type.cpp"
	"src/lang/scope.cpp"
	"src/lang/func_def.cpp"
	"src/lang/return.cpp"
	"src/lang/literal.cpp"
	"src/lang/root_statement.cpp"
	"src/lang/func_decl.cpp"
)

target_compile_options (${PROJECT_NAME} PRIVATE -g -Wall -Wextra -Wpedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic)
target_include_directories (${PROJECT_NAME} PRIVATE "include")

set_target_properties (${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED TRUE
    VERSION ${PROJECT_VERSION}
	EXPORT_COMPILE_COMMANDS TRUE
)

