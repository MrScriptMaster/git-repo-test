#-------------------------------------------
# Explicit definitions of comma and space characters
#-------------------------------------------
chr_comma := ,
chr_empty :=
chr_space := $(chr_empty) $(chr_empty)

srcp = $(source_module_prefix)
trgp = $(LPROD)$(component_id)

#-------------------------------------------
# Define load module:
#-------------------------------------------
ifneq ($(load_module_name),)
load = $(LOADDIR)/$(trgp)$(load_module_name)
endif
#-------------------------------------------

iobj_mpref = $(OBJDIR)/$(LPROD)
obj_mpref  = $(iobj_mpref)$(component_id)
obj_mask   = $(obj_mpref)%.o

src_mpref = $(CURDIR)/src/$(srcp)

#-----------------------------------------------------------------------------------
# Define C++ build object modules:
#-----------------------------------------------------------------------------------
ifndef cpp_sources
cpp_objs = $(patsubst $(src_mpref)%.cpp,$(obj_mask),$(wildcard $(src_mpref)*.cpp))
else
ifneq ($(cpp_sources),-)
cpp_objs = $(cpp_sources:%=$(obj_mask))
endif
endif

#-----------------------------------------------------------------------------------
# Define Assembler build object modules:
#-----------------------------------------------------------------------------------
ifndef asm_sources
asm_objs = $(patsubst $(src_mpref)%.s,$(obj_mask),$(wildcard $(src_mpref)*.s))
else
ifneq ($(asm_sources),-)
asm_objs = $(asm_sources:%=$(obj_mask))
endif
endif
#-----------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------
# Define import object modules:
#-----------------------------------------------------------------------------------
ifdef load
ifneq ($(import_obj_libs),)
imp_objs = $(foreach obj_lib,$(import_obj_libs),$($(obj_lib)_objs:%=$(iobj_mpref)$($(obj_lib)_component_id)%.o))
endif
endif
#-----------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------
# Define all object and dependency modules:
#-----------------------------------------------------------------------------------
objs = $(cpp_objs) $(asm_objs) $(imp_objs)
deps = $(objs:$(iobj_mpref)%.o=$(DEPDIR)/$(LPROD)%.d)
#-----------------------------------------------------------------------------------

#-------------------------------------------
# Define all targets:
#-------------------------------------------
ifdef load
targets = $(load)
else
targets = $(cpp_objs) $(asm_objs)
endif
#-------------------------------------------

#-------------------------------------------
# Main rule: "all":
#-------------------------------------------
ifdef targets

ifdef asm_objs
wrk_asm_includes = $(notdir $(CURDIR)) $(add_component_asm_includes)
endif

.PHONY: all $(wrk_asm_includes)
all: $(wrk_asm_includes) $(targets)

$(wrk_asm_includes):
	@$(MAKE) -C $(PRDDIR)/$@ -f $(MAKEINC_DIR)/build_wrk_asm_includes.mk

endif
#-------------------------------------------

include $(MAKEINC_DIR)/build_default_options.mk

UCMP = $(shell echo $(component_id) | tr [:lower:] [:upper:])
LCMP = $(shell echo $(component_id) | tr [:upper:] [:lower:])

#-------------------------------------------
# Common current file names:
#-------------------------------------------
trg_base_name = $(basename $(@F))

curr_dep  = $(DEPDIR)/$(trg_base_name).d
curr_exp  = $(EXPDIR)/$(trg_base_name).x
curr_lstc = $(LSTCDIR)/$(trg_base_name).lstc
curr_lst  = $(LSTADIR)/$(trg_base_name).lst
curr_lstl = $(LMAPDIR)/$(trg_base_name).lstlnk

curr_mvs_obj   = //'$(MVS_OBJ)($(trg_base_name))'
curr_mvs_load  = //'$(MVS_LOAD)($(trg_base_name))'
curr_mvs_lstc  =    $(MVS_LIST)($(trg_base_name))
curr_mvs_lst   =    $(MVS_ALST)($(trg_base_name))
curr_mvs_lstl  =    $(MVS_LMAP)($(trg_base_name))
curr_mvs_adata = //'$(ADATA_DATASET)($(trg_base_name))'
#-------------------------------------------

src_prereq_mask = $(obj_mask) : $(src_mpref)%

#-------------------------------------------
# Rule to compile C++ source modules:
#-------------------------------------------
ifneq ($(cpp_objs),)

cpp_includes = $(addprefix -I,$(prd_includes) $(CURDIR)/include $(add_component_cpp_includes:%=$(PRDDIR)/%/include) $(add_cpp_includes))
cpp_options  = $(cpp_default_options) $(add_cpp_options)

$(cpp_objs): $(src_prereq_mask).cpp
	@$(TIMEPRINT) ' C++ compilation: $(notdir $<)' $(COLOR_RESET);\
	xlC -+ -c $(cpp_includes) -qmakedep -MF $(curr_dep) -Wc,"$(cpp_options) LIST($(curr_lstc))" -o $@ $<;\
		rc=$$?; if [ $$rc -gt 4 ]; then; exit $$rc; fi
	@cp -A $@ "$(curr_mvs_obj)"
	@if [ "$(COPY_LISTINGS_TO_MVS)" -ne "0" ]; then; tso "oget '$(curr_lstc)' '$(curr_mvs_lstc)'"; fi

endif
#-------------------------------------------

#-------------------------------------------
# Rule to compile Assembler source modules:
#-------------------------------------------
ifneq ($(asm_objs),)

asm_includes = $(addprefix -I ,$(WRKINCDIR) $(wrk_asm_includes:%=$(WRKINCDIR)/%) $(add_asm_includes))

ifndef add_asm_options
asm_options = $(asm_default_options)
else
asm_options = $(subst $(chr_space),,$(asm_default_options)$(chr_comma)$(add_asm_options))
endif

$(asm_objs): $(src_prereq_mask).s
	@$(TIMEPRINT) ' Assembling: $(notdir $<)' $(COLOR_RESET);\
	as $(asm_includes) --"$(asm_options)" -am=$(curr_lst) $(if $(DEBUG_MODE),--gadata="$(curr_mvs_adata)") -o $@ $<;\
		rc=$$?; if [ $$rc -gt 4 ]; then; exit $$rc; fi;\
	$(ASMDEPPGM) $(curr_lst) $(curr_dep);\
		rc=$$?; if [ $$rc -gt 4 ]; then; $(RM) $@; exit $$rc; fi
	@cp -A $@ "$(curr_mvs_obj)"
	@if [ "$(COPY_LISTINGS_TO_MVS)" -ne "0" ]; then; tso "oget '$(curr_lst)' '$(curr_mvs_lst)'"; fi

endif
#-------------------------------------------

ifdef load
#-----------------------------------------------------------
# Rule with error message for required not built components:
#-----------------------------------------------------------
ifneq ($(imp_objs),)
$(imp_objs):
	@echo 'ERROR: Object module was not built or is obsolete: $(@F).';\
	echo  '       Its component should be (re)built before current component.';\
	exit 8
endif
#-----------------------------------------------------------
# Rule to link:
#-----------------------------------------------------------

#
# Set _LD_ACCEPTABLE_RC environment variable to ZERO in order to make
# uss linker return RC=4 (instead of RC=0) on WARNING
#
export _LD_ACCEPTABLE_RC = 0

lnk_syslibs = $(addprefix -S ,"//'CEE.SCEELKEX'" "//'CEE.SCEELKED'" "//'CEE.SCEECPP'" "//'ISP.SISPLOAD'")

ifndef add_lnk_options
lnk_options = $(lnk_default_options)
else
lnk_options = $(subst $(chr_space),,$(lnk_default_options)$(chr_comma)$(add_lnk_options))
endif

ifeq ($(load_module_entry),)
trg_entry = CEESTART
else
trg_entry = $(trgp)$(load_module_entry)
endif

$(load): $(objs)
	@$(TIMEPRINT) ' Linking: $(@F)' $(COLOR_RESET);\
	ld -Vv -e '$(trg_entry)' $(lnk_syslibs) -b"$(lnk_options)" -x $(curr_exp) -o $@ $(objs) "//'CEE.SCEELIB(C128N)'" > $(curr_lstl);\
		rc=$$?; $(RM) $(curr_exp); if [ $$rc -gt 4 ]; then; $(RM) $@; exit $$rc; fi
	@cp -A -X $@ "$(curr_mvs_load)"
	@if [ "$(COPY_LISTINGS_TO_MVS)" -ne "0" ]; then; tso "oget '$(curr_lstl)' '$(curr_mvs_lstl)'"; fi

#-----------------------------------------------------------
endif

-include $(deps)
