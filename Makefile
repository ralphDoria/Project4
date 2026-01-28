# Define the tools
AR=ar
CC=gcc
CXX=g++

# Define the directories
INC_DIR				= ./include
SRC_DIR				= ./src
TESTSRC_DIR			= ./testsrc
BIN_DIR				= ./bin
OBJ_DIR				= ./obj
LIB_DIR				= ./lib
TESTOBJ_DIR			= ./testobj
TESTBIN_DIR			= ./testbin
TESTCOVER_DIR		= ./htmlcov

# Define the flags for compilation/linking
DEFINES				=
INCLUDE				= -I $(INC_DIR)
ARFLAGS				= rcs
CFLAGS				= -Wall
CPPFLAGS			= --std=c++20
LDFLAGS				=

TEST_CFLAGS			= $(CFLAGS) -O0 -g --coverage
TEST_CPPFLAGS		= $(CPPFLAGS) -fno-inline
TEST_LDFLAGS		= $(LDFLAGS) -lgtest -lgtest_main -lpthread

# Define the object files
SVG_OBJ				= $(OBJ_DIR)/svg.o

TEST_SVG_OBJ		= $(TESTOBJ_DIR)/svg.o
TEST_SVG_TEST_OBJ	= $(TESTOBJ_DIR)/SVGTest.o
TEST_OBJ_FILES		= $(TEST_SVG_OBJ) $(TEST_SVG_TEST_OBJ)

TEST_STRSINK_OBJ		= $(TESTOBJ_DIR)/StringDataSink.o
TEST_STRSINK_TEST_OBJ	= $(TESTOBJ_DIR)/StringDataSinkTest.o
TEST_STRSINK_OBJ_FILES	= $(TEST_STRSINK_OBJ) $(TEST_STRSINK_TEST_OBJ)

TEST_STRSRC_OBJ			= $(TESTOBJ_DIR)/StringDataSource.o
TEST_STRSRC_TEST_OBJ 	= $(TESTOBJ_DIR)/StringDataSourceTest.o
TEST_STRSRC_OBJ_FILES	= $(TEST_STRSRC_OBJ) $(TEST_STRSRC_TEST_OBJ)

TEST_SVGWRITER_OBJ		= $(TESTOBJ_DIR)/SVGWriter.o
TEST_SVGWRITER_TEST_OBJ	= $(TESTOBJ_DIR)/SVGWriterTest.o
TEST_SVGWRITER_OBJ_FILES= $(TEST_SVGWRITER_OBJ) $(TEST_SVGWRITER_TEST_OBJ) $(TEST_STRSINK_OBJ) $(SVGLIB_TARGET)

# XML READER TEST objects
TEST_XMLREADER_OBJ		= $(TESTOBJ_DIR)/XMLReader.o
TEST_XML_TEST_OBJ		= $(TESTOBJ_DIR)/XMLTest.o
TEST_XML_OBJ_FILES		= $(TEST_XMLREADER_OBJ) $(TEST_XML_TEST_OBJ) $(TEST_STRSRC_OBJ)
TEST_XML_TARGET			= $(TESTBIN_DIR)/testxml
# Link Expat for XML
TEST_XML_LDFLAGS		= $(TEST_LDFLAGS) -lexpat


# Define the targets
SVGLIB_TARGET			= $(LIB_DIR)/libsvg.a

TEST_SVG_TARGET			= $(TESTBIN_DIR)/testsvg
TEST_STRSINK_TARGET 	= $(TESTBIN_DIR)/teststrdatasink
TEST_STRSRC_TARGET 		= $(TESTBIN_DIR)/teststrdatasource
TEST_SVGWRITER_TARGET   = $(TESTBIN_DIR)/testsvgwriter


all: directories run_svgtest make_svglib run_sinktest run_sourcetest run_xmltest run_svgwritertest gen_html

run_svgtest: $(TEST_SVG_TARGET)
	$(TEST_SVG_TARGET)

make_svglib: $(SVGLIB_TARGET)

run_sinktest: $(TEST_STRSINK_TARGET)
	$(TEST_STRSINK_TARGET)

run_sourcetest: $(TEST_STRSRC_TARGET)
	$(TEST_STRSRC_TARGET)

run_svgwritertest: $(TEST_SVGWRITER_TARGET)
	$(TEST_SVGWRITER_TARGET)

run_xmltest: $(TEST_XML_TARGET)
	$(TEST_XML_TARGET)


gen_html:
	lcov --capture --directory . --output-file $(TESTCOVER_DIR)/coverage.info --ignore-errors inconsistent,source
	lcov --remove $(TESTCOVER_DIR)/coverage.info '/usr/*' '*/testsrc/*' --output-file $(TESTCOVER_DIR)/coverage.info
	genhtml $(TESTCOVER_DIR)/coverage.info --output-directory $(TESTCOVER_DIR)

$(TEST_SVG_TARGET): $(TEST_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_SVG_TARGET)

$(SVGLIB_TARGET): $(SVG_OBJ)
	$(AR) $(ARFLAGS) $(SVGLIB_TARGET) $(SVG_OBJ)

$(TEST_STRSINK_TARGET): $(TEST_STRSINK_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_STRSINK_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_STRSINK_TARGET)

$(TEST_STRSRC_TARGET): $(TEST_STRSRC_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_STRSRC_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_STRSRC_TARGET)

$(TEST_SVGWRITER_TARGET): $(TEST_SVGWRITER_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_SVGWRITER_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_SVGWRITER_TARGET)

# testxml
$(TEST_XML_TARGET): $(TEST_XML_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_XML_OBJ_FILES) $(TEST_XML_LDFLAGS) -o $(TEST_XML_TARGET)


$(TEST_SVG_OBJ): $(SRC_DIR)/svg.c
	$(CC) $(TEST_CFLAGS) $(DEFINES) $(INCLUDE) -c $(SRC_DIR)/svg.c -o $(TEST_SVG_OBJ)

$(SVG_OBJ): $(SRC_DIR)/svg.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE) -c $(SRC_DIR)/svg.c -o $(SVG_OBJ)

$(TEST_SVG_TEST_OBJ): $(TESTSRC_DIR)/SVGTest.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $(TESTSRC_DIR)/SVGTest.cpp -o $(TEST_SVG_TEST_OBJ)

$(TESTOBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

$(TESTOBJ_DIR)/%.o: $(TESTSRC_DIR)/%.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(TESTOBJ_DIR)
	mkdir -p $(TESTBIN_DIR)
	mkdir -p $(TESTCOVER_DIR)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	rm -rf $(TESTOBJ_DIR)
	rm -rf $(TESTBIN_DIR)
	rm -rf $(TESTCOVER_DIR)

