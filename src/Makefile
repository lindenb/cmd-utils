SHELL=/bin/bash
CCFLAGS=-Wall -g
CC=g++
.PHONY:all

define has_lib
	echo -e "#include <$(1)>\nint main(int argc,char** argv) { return 0;}" > $(addsuffix .tmp.c,$@) && \
		gcc -o  $(addsuffix .tmp.exe,$@) $(addsuffix .tmp.c,$@) $(3) 2> $(addsuffix .err,$@) && echo "#define $(2) 1" >> $@ || echo "#define $(2) 0" >> $@ 
	rm -f $(addsuffix .tmp.c,$@) $(addsuffix .tmp.exe,$@) $(addsuffix .err,$@)
endef


# 1 name
# 2 clags
# 3 libs
define compile0

$(1) : $$(addprefix $(1).,cpp xml)  xml2h.xsl preproc.xsl config.h
	xsltproc preproc.xsl $$(filter %.xml,$$^) | xsltproc xml2h.xsl -  > $$(addsuffix .tab.h,$$(basename $$<))
	$$(CC) $(2) -o $$@ $$(CCFLAGS) $$< $(3)

endef

APPS0 = colsmd5 linearizefasta tab2spaces groupby table2yxv cutbyname yxv2table math-sum
APPS = $(APPS0) treepack

all:  $(APPS)

config.h : 
	echo '#ifndef CONFIG_H' > $@
	echo '#define CONFIG_H' >> $@
	$(call has_lib,cairo/cairo.h,HAS_LIB_CAIRO,-lcairo)
	echo '#endif' >> $@
	

$(eval $(foreach A,${APPS0},$(call compile0,${A},,)))
$(eval $(call compile0,treepack,`xml2-config --cflags`,`xml2-config --libs`))


z:	$(CC) $(CCFLAGS) test.cpp
	./a.out "[]"  " [ ] "  " {}" '[12332]' '[false]' '[null]' '["azd"]' '["a",null,09]' '{"a":1,"b":null,"c":9872}' \
		'{"a":[098,null,false,true,{}],"b":{"c":[null]}}'



