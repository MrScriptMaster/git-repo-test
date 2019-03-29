################################################################################
# of                                                                           #
################################################################################
OF_PATH_PREFIX := ./include/third_party/of
OF_MODULES := 3d app communication events gl graphics math sound types utils video
OF_INCLUDES := $(OF_PATH_PREFIX) $(sort $(addprefix $(OF_PATH_PREFIX)/, $(OF_MODULES)))
OF_LIB_PATH := ./lib/of/linux64
OF_LIB_NAME := -lopenFrameworks

################################################################################
# fmodex                                                                       #
################################################################################
FMODEX_INCLUDE := ./lib/fmodex/include
FMODEX_LIB_PATH := ./lib/fmodex/lib/linux64/libfmodex.so

################################################################################
# tess2                                                                        #
################################################################################
TESS2_INCLUDE := ./lib/tess2/include
TESS2_LIB_PATH := ./lib/tess2/lib/linux64
TESS2_LIB_NAME := -ltess2

################################################################################
# utf8                                                                         #
################################################################################
UTF8_INCLUDE := ./lib/utf8/include

################################################################################
# json                                                                         #
################################################################################
JSON_INCLUDE := ./lib/json/include

################################################################################
# kiss                                                                         #
################################################################################
KISS_INCLUDE := ./lib/kiss/include
KISS_LIB_PATH := ./lib/kiss/lib/linux64
KISS_LIB_NAME := -lkiss

################################################################################
# glm                                                                          #
################################################################################
GLM_INCLUDE := ./lib/glm/include

################################################################################
# Input parameters                                                             #
################################################################################
SOURCE_PREFIX := ./src
OBJ_PREFIX    := ./.obj
COMPILER      := g++
LINK          := g++
INCLUDES      := ./include ./include/third_party $(OF_INCLUDES) $(FMODEX_INCLUDE) $(TESS2_INCLUDE) $(UTF8_INCLUDE) \
	$(JSON_INCLUDE) $(KISS_INCLUDE) $(GLM_INCLUDE)
ADDITIONAL_INCLUDES := `pkg-config gstreamer-app-1.0 --cflags-only-I`
LFLAGS        := -pthread
CFLAGS        := -pthread
DEFINE        := -DGLM_ENABLE_EXPERIMENTAL
LIBS          := $(OF_LIB_PATH) $(TESS2_LIB_PATH) $(KISS_LIB_PATH)
L_LIBS        := $(OF_LIB_NAME) $(FMODEX_LIB_PATH) $(TESS2_LIB_NAME) $(KISS_LIB_NAME) \
	`pkg-config gstreamer-app-1.0 gstreamer-video-1.0 glew x11 glu glfw3 cairo fontconfig openal opencv freetype2 assimp --libs` \
	-ldl -lboost_system -lboost_filesystem -lcurl -luriparser -lgtk-3 -lglut -lfreeimage \
	-lassimp -lsndfile -lmpg123
APP_NAME      := application
RM            := rm -rf

################################################################################
# Generate lists                                                               #
################################################################################
OBJ    := $(sort $(patsubst $(SOURCE_PREFIX)/%, %, $(patsubst %.cpp, %.o, $(wildcard $(SOURCE_PREFIX)/*.cpp))))
P_OBJ  := $(sort $(addprefix $(OBJ_PREFIX)/, $(patsubst $(SOURCE_PREFIX)/%, %, $(patsubst %.cpp, %.o, $(wildcard $(SOURCE_PREFIX)/*.cpp)))))
DFILES := $(notdir $(sort $(patsubst %.cpp, %.d, $(wildcard $(SOURCE_PREFIX)/*.cpp))))

vpath %.cpp $(SOURCE_PREFIX)
vpath %.c $(SOURCE_PREFIX)
vpath %.d $(OBJ_PREFIX)
vpath %.o $(OBJ_PREFIX)
vpath %.h $(INCLUDES)
vpath %.hpp $(INCLUDES)

################################################################################
# Main targets                                                                 #
################################################################################
.PHONY: all make_dirs clean
all: make_dirs $(DFILES) $(OBJ) $(APP_NAME)

make_dirs:
	@mkdir -p $(OBJ_PREFIX)
	@mkdir -p include src

$(APP_NAME): $(P_OBJ)
	$(LINK) $^ $(LFLAGS) -o $@ $(addprefix -L,$(LIBS)) $(L_LIBS)

clean:
	$(RM) $(OBJ_PREFIX)/*.d
	$(RM) $(OBJ_PREFIX)/*.o
	$(RM) $(APP_NAME)

################################################################################
# Rules                                                                        #
################################################################################
%.o: %.cpp
	$(COMPILER) -c $(CFLAGS) $(DEFINE) $(addprefix -I, $(INCLUDES)) $(ADDITIONAL_INCLUDES) -o $(OBJ_PREFIX)/$@ $<

%.d: %.cpp
	$(COMPILER) -M $(CFLAGS) $(DEFINE) $(addprefix -I, $(INCLUDES)) $(ADDITIONAL_INCLUDES) $< -o $(OBJ_PREFIX)/$@  

include $(wildcard $(OBJ_PREFIX)/*.d)
