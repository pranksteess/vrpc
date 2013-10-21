#gcc version
CC  = gcc
CXX = g++
GCCVER := $(shell $(CC) -dumpversion | awk -F. '{ print $$1"."$$2}' )

INC += -I./spp_incl/
#LIB += -lsppasync -lasync_epoll -lcore

#BUILD_DIR=../../build 
OBJ_DIR=./obj
LIB_DIR=.

LIB_NAME=spp_module_test.so 

TARGET = $(LIB_DIR)/$(LIB_NAME)

	
OBJ = $(OBJ_DIR)/GetInfo.o \
      $(OBJ_DIR)/GetExtInfo.o \
      $(OBJ_DIR)/GetState.o \
      $(OBJ_DIR)/GetState2.o \
      $(OBJ_DIR)/UpdateState.o \
      $(OBJ_DIR)/UpdateData.o \
      $(OBJ_DIR)/msg.o \
      $(OBJ_DIR)/service.o \

OBJ_EXT= .o
CXXSRC_EXT= .cpp
CSRC_EXT= .c

CFLAGS  = -g -fPIC -shared -Wall -O2 -pipe -fno-ident -MMD -D_GNU_SOURCE -D_REENTRANT
#ARCH = $(shell $(CC) $(DEBUG) -E -DARCHTEST ../../build/conftest.c|grep 86)
#ifeq ($(ARCH),i386)
#    CFLAGS += $(shell ../../build/conftest.sh "$(CC)" -march=pentium3)
#else
#ifeq ($(ARCH),x86_64)
#else
#echo $(error Unsupported Compiler, CC=$(CC) ARCH=$(ARCH)))
#endif
#endif

#CXXFLAGS=-fno-rtti
CXXFLAGS+=$(CFLAGS)


$(OBJ_DIR)/%$(OBJ_EXT): %$(CXXSRC_EXT)
	@echo
	@echo "Compiling $< ==> $@..."
	@-mkdir -p $(OBJ_DIR)
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%$(OBJ_EXT): %$(CSRC_EXT)
	@echo
	@echo "Compiling $< ==> $@..."
	@-mkdir -p $(OBJ_DIR)
	$(CC) $(INC) $(CFLAGS) -c $< -o $@


all:$(TARGET)

$(TARGET) : $(OBJ)
	@echo
	@echo "Building $^ ==> $@..."
	@-mkdir -p $(LIB_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB)
	install *.so ../../bin

clean:
	@echo
	@echo "Cleaning..." 
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)	

copy: 
	@rm ./spp_incl/ -rf
	@-mkdir ./spp_incl
	@cp ../comm/spp_version.h ./spp_incl/.
	@cp ../comm/serverbase.h ./spp_incl/.
	@cp ../comm/tbase/atomic*.h ./spp_incl/.
	@cp ../comm/tbase/myatomic*.h ./spp_incl/.
	@cp ../comm/tbase/list.h ./spp_incl/.
	@cp ../comm/tbase/tcommu.h ./spp_incl/.
	@cp ../comm/tbase/tlog.h  ./spp_incl/.
	@cp ../comm/tbase/tprocmon.h  ./spp_incl/.
	@cp ../comm/tbase/tstat.h  ./spp_incl/.
	@cp ../comm/tbase/tstat_policy.h  ./spp_incl/.
	@cp ../async/asyn_api.h ./spp_incl/.
	@cp ../async/spp_async_interface.h ./spp_incl/.
	@cp ../async_frame/CommDef.h ./spp_incl/.
	@cp ../async_frame/AsyncFrame.h ./spp_incl/.
	@cp ../async_frame/IState.h ./spp_incl/.
	@cp ../async_frame/IAction.h ./spp_incl/.
	@cp ../async_frame/ActionSet.h ./spp_incl/.
	@cp ../async_frame/ActionInfo.h ./spp_incl/.
	@cp ../async_frame/NonStateActionInfo.h ./spp_incl/.
	@cp ../async_frame/StateActionInfo.h ./spp_incl/.
	@cp ../async_frame/AntiParalActionInfo.h ./spp_incl/.
	@cp ../async_frame/MsgBase.h ./spp_incl/.
	@cp ../async_frame/PtrMgr.h ./spp_incl/.
	@cp ../async_frame/SEXInterface.h ./spp_incl/.


