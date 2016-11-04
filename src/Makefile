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
# 2 other
# 3 clags
# 4 libs
define compile0

$(1) : $$(addprefix $(1).,cpp xml)  xml2h.xsl preproc.xsl config.h $(2)
	xsltproc preproc.xsl $$(filter %.xml,$$^) | xsltproc xml2h.xsl -  > $$(addsuffix .tab.h,$$(basename $$<))
	$$(CC) $(3) -o $$@ $$(CCFLAGS) $(2) $$< $$(filter %.o,$$^) $(4)

endef

# 1 name
# 2 clags
define lexyacc

$(1).bison.o : $(1).bison.c  $(1).bison.hh $(1).flex.h $(1).m4.hh
	$$(CC) $(2) -c -o $$@ $$< 

$(1).flex.o : $(1).flex.c $(1).flex.h  $(1).bison.h $(1).m4.hh
	$$(CC) $(2) -c -o $$@ $$< 

$(1).flex.c : $(1).flex.h
	touch -c $$@

$(1).flex.h : $(1).l
	flex --reentrant -P $(1)  --header-file=$$@ -o $(1).flex.c $$<

$(1).bison.c : $(1).bison.h
	touch -c $$@

$(1).bison.h : $(1).y
	bison --locations -Wall --defines=$$@ --output=$(1).bison.c -b $(1) -p $(1) $$<

$(1).m4.hh : parser.m4
	m4 -DTOOL=$(1) $$<  > $$@

endef

APPS0 = combination wordcount headtail dispatch escape-xml escape-c colsmd5 linearizefasta tab2spaces groupby table2yxv cutbyname yxv2table math-sum math-mean math-median
APPS = $(APPS0) treepack json_array_foreach

all:  $(APPS)

config.h : 
	echo '#ifndef CONFIG_H' > $@
	echo '#define CONFIG_H' >> $@
	$(call has_lib,cairo/cairo.h,HAS_LIB_CAIRO,-lcairo)
	echo '#endif' >> $@
	

$(eval $(foreach A,${APPS0},$(call compile0,${A},,,)))
$(eval $(call compile0,treepack,,`xml2-config --cflags`,`xml2-config --libs`))


json_array_foreach: $(addprefix json_array_foreach, .l .y)
	flex --header-file=$(addsuffix .flex.h,$@) -o $(addsuffix .flex.c,$@) $(filter %.l,$^)
	bison --locations --defines=$(addsuffix .bison.h,$@) --output=$(addsuffix .bison.c,$@)  $(filter %.y,$^)
	$(CC) -c $(CCFLAGS) -o $(addsuffix .flex.o,$@) $(addsuffix .flex.c,$@)
	$(CC) -c $(CCFLAGS) -o $(addsuffix .bison.o,$@) $(addsuffix .bison.c,$@)	


z:	$(CC) $(CCFLAGS) test.cpp
	./a.out "[]"  " [ ] "  " {}" '[12332]' '[false]' '[null]' '["azd"]' '["a",null,09]' '{"a":1,"b":null,"c":9872}' \
		'{"a":[098,null,false,true,{}],"b":{"c":[null]}}'



