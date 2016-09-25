CCFLAGS=-Wall -O3
CC=g++
all: yxv2table.cpp yxv2table.xml
	xsltproc preproc.xsl yxv2table.xml | xsltproc xml2h.xsl -  > yxv2table.tab.h
	$(CC) $(CCFLAGS) $<

