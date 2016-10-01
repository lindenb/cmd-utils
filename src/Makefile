CCFLAGS=-Wall -g
CC=g++
.PHONY:all

define compile0

$(1) : $$(addprefix $(1).,cpp xml)  xml2h.xsl preproc.xsl
	xsltproc preproc.xsl $$(filter %.xml,$$^) | xsltproc xml2h.xsl -  > $$(addsuffix .tab.h,$$(basename $$<))
	$$(CC) -o $$@ $$(CCFLAGS) $$<

endef

APPS=cutbyname yxv2table math-sum

all:  ${APPS}

$(eval $(foreach A,${APPS},$(call compile0,${A})))



z:	$(CC) $(CCFLAGS) test.cpp
	./a.out "[]"  " [ ] "  " {}" '[12332]' '[false]' '[null]' '["azd"]' '["a",null,09]' '{"a":1,"b":null,"c":9872}' \
		'{"a":[098,null,false,true,{}],"b":{"c":[null]}}'



