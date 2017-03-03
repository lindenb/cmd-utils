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
	xsltproc preproc.xsl $$(filter %.xml,$$^) | xsltproc xml2md.xsl - > ../docs/$$(addsuffix .md,$$(notdir $$(basename $$<)))
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

APPS0 =  categorize csv2tsv tojson fromto escape-http math-minmax math-normalize waitforfiles shuffle hershey combination wordcount headtail dispatch escape-xml escape-c colsmd5 linearizefasta tab2spaces groupby table2yxv cutbyname yxv2table math-sum math-mean math-median
APPS = $(APPS0) wordcloud table2xml xmlpath treepack json_array_foreach json_object_foreach

all:  $(APPS)

config.h : 
	echo '#ifndef CONFIG_H' > $@
	echo '#define CONFIG_H' >> $@
	$(call has_lib,cairo/cairo.h,HAS_LIB_CAIRO,-lcairo)
	echo '#endif' >> $@
	

$(eval $(foreach A,${APPS0},$(call compile0,${A},,,)))
$(eval $(call compile0,treepack,,`xml2-config --cflags`,`xml2-config --libs`))
$(eval $(call compile0,table2xml,,`xml2-config --cflags`,`xml2-config --libs`))
$(eval $(call compile0,xmlpath,,`xml2-config --cflags`,`xml2-config --libs`))
$(eval $(call compile0,wordcloud,,`xml2-config --cflags`,`xml2-config --libs`))

json_array_foreach: $(addprefix json_array_foreach, .l .y .xml .cpp)
	flex -P $@ --header-file=$(addsuffix .flex.h,$@) -o $(addsuffix .flex.c,$@) $(filter %.l,$^)
	bison -p $@ --locations --defines=$(addsuffix .bison.h,$@) --output=$(addsuffix .bison.c,$@)  $(filter %.y,$^)
	$(CC) -c $(CCFLAGS) -o $(addsuffix .flex.o,$@) $(addsuffix .flex.c,$@)
	$(CC) -c $(CCFLAGS) -o $(addsuffix .bison.o,$@) $(addsuffix .bison.c,$@)
	xsltproc preproc.xsl $(filter %.xml,$^) | xsltproc xml2h.xsl -  > $(addsuffix .tab.h,$@)
	$(CC) $(CCFLAGS) -o $@  $(addprefix $@,.cpp .flex.o .bison.o)
		
json_object_foreach: $(addprefix json_object_foreach, .y .xml .cpp) json_array_foreach.l
	sed 's/json_array_foreach/json_object_foreach/g' json_array_foreach.l > json_object_foreach.l
	flex -P $@ --header-file=$(addsuffix .flex.h,$@) -o $(addsuffix .flex.c,$@) json_object_foreach.l
	bison  -p $@ --locations --defines=$(addsuffix .bison.h,$@) --output=$(addsuffix .bison.c,$@)  $(filter %.y,$^)
	$(CC) -c $(CCFLAGS) -o $(addsuffix .flex.o,$@) $(addsuffix .flex.c,$@)
	$(CC) -c $(CCFLAGS) -o $(addsuffix .bison.o,$@) $(addsuffix .bison.c,$@)
	xsltproc preproc.xsl $(filter %.xml,$^) | xsltproc xml2h.xsl -  > $(addsuffix .tab.h,$@)
	$(CC) $(CCFLAGS) -o $@  $(addprefix $@,.cpp .flex.o .bison.o)
	rm json_object_foreach.l

json2xml: $(addprefix json2xml, .y .xml .cpp) json_array_foreach.l
	sed 's/json_array_foreach/json2xml/g' json_array_foreach.l > json2xml.l
	flex -P $@ --header-file=$(addsuffix .flex.h,$@) -o $(addsuffix .flex.c,$@) json2xml.l
	bison  -p $@ --locations --defines=$(addsuffix .bison.h,$@) --output=$(addsuffix .bison.c,$@)  $(filter %.y,$^)
	$(CC) -c $(CCFLAGS) `xml2-config --cflags` -o $(addsuffix .flex.o,$@) $(addsuffix .flex.c,$@)
	$(CC) -c $(CCFLAGS) `xml2-config --cflags` -o $(addsuffix .bison.o,$@) $(addsuffix .bison.c,$@)
	xsltproc preproc.xsl $(filter %.xml,$^) | xsltproc xml2h.xsl -  > $(addsuffix .tab.h,$@)
	$(CC) $(CCFLAGS) `xml2-config --cflags` -o $@  $(addprefix $@,.cpp .flex.o .bison.o) `xml2-config --libs` 
	rm json2xml.l



json_tree.a : 	$(addprefix json_tree, .l .y) json_node.hh
	flex -P $(basename $@) --header-file=$(addsuffix .flex.h,$(basename $@)) -o $(addsuffix .flex.c,$(basename $@))  $(addsuffix .l,$(basename $@))
	bison  -p $(basename $@) --locations --defines=$(addsuffix .bison.h,$(basename $@)) --output=$(addsuffix .bison.c,$(basename $@))  $(filter %.y,$^)
	$(CC) -c $(CCFLAGS) -o $(addsuffix .flex.o,$(basename $@)) $(addsuffix .flex.c,$(basename $@))
	$(CC) -c $(CCFLAGS) -o $(addsuffix .bison.o,$(basename $@)) $(addsuffix .bison.c,$(basename $@))
	ar rcs $@ $(addsuffix .flex.o,$(basename $@)) $(addsuffix .bison.o,$(basename $@))

z:	$(CC) $(CCFLAGS) test.cpp
	./a.out "[]"  " [ ] "  " {}" '[12332]' '[false]' '[null]' '["azd"]' '["a",null,09]' '{"a":1,"b":null,"c":9872}' \
		'{"a":[098,null,false,true,{}],"b":{"c":[null]}}'



