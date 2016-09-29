CCFLAGS=-Wall -g
CC=g++
.PHONY:all
all:  cutbyname yxv2table

cutbyname: $(addprefix cutbyname.,cpp xml)
	xsltproc preproc.xsl $(filter %.xml,$^) | xsltproc xml2h.xsl -  > $(addsuffix .tab.h,$(basename $<))
	$(CC) -o $@ $(CCFLAGS) $<

yxv2table: $(addprefix yxv2table.,cpp xml)
	xsltproc preproc.xsl $(filter %.xml,$^) | xsltproc xml2h.xsl -  > $(addsuffix .tab.h,$(basename $<))
	$(CC) -o $@ $(CCFLAGS) $<



z:	$(CC) $(CCFLAGS) test.cpp
	./a.out "[]"  " [ ] "  " {}" '[12332]' '[false]' '[null]' '["azd"]' '["a",null,09]' '{"a":1,"b":null,"c":9872}' \
		'{"a":[098,null,false,true,{}],"b":{"c":[null]}}'



