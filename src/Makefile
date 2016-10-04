SHELL=/bin/bash
CCFLAGS=-Wall -g
CC=g++
.PHONY:all

HAS_CAIRO=$(shell  l)

define compile0

$(1) : $$(addprefix $(1).,cpp xml)  xml2h.xsl preproc.xsl config.h
	xsltproc preproc.xsl $$(filter %.xml,$$^) | xsltproc xml2h.xsl -  > $$(addsuffix .tab.h,$$(basename $$<))
	$$(CC) -o $$@ $$(CCFLAGS) $$<

endef

APPS=treepack tab2spaces groupby table2yxv cutbyname yxv2table math-sum

all:  ${APPS}
	echo "${HAS_CAIRO}"

config.h : 
	echo '#ifndef CONFIG_H' > $@
	echo '#define CONFIG_H' >> $@
	ldconfig -p | grep -m1  libcairo | awk 'END {printf("#define HAS_LIB_CAIRO %d\n",(NR>0) );}'>> $@
	echo '#endif' >> $@
	

$(eval $(foreach A,${APPS},$(call compile0,${A})))



z:	$(CC) $(CCFLAGS) test.cpp
	./a.out "[]"  " [ ] "  " {}" '[12332]' '[false]' '[null]' '["azd"]' '["a",null,09]' '{"a":1,"b":null,"c":9872}' \
		'{"a":[098,null,false,true,{}],"b":{"c":[null]}}'



