<?xml version='1.0'  encoding="ISO-8859-1"?>
<xsl:stylesheet
	xmlns:xsl='http://www.w3.org/1999/XSL/Transform'
	xmlns:time="http://exslt.org/dates-and-times"
	version='1.1'
	>
<xsl:output method='text' encoding="UTF-8"/>

<xsl:template match="cmd" >#<xsl:value-of select="@name"/>

##Motivation

<xsl:apply-templates select="description"/>

##Options

<xsl:for-each select="options/option">
<xsl:text>  * </xsl:text>
<xsl:if test="@opt">-<xsl:value-of select="@opt"/></xsl:if>
<xsl:if test="@opt and @longopt">|</xsl:if>
<xsl:if test="@longopt">--<xsl:value-of select="@longopt"/></xsl:if>
<xsl:text>   </xsl:text>
<xsl:apply-templates select="description"/>

<xsl:choose>
	<xsl:when test="@type='enum'">. Possible values: [<xsl:for-each select="item"><xsl:if test="position()&gt;1">|</xsl:if><xsl:value-of select="@name"/></xsl:for-each>]. Default: "<xsl:value-of select="@name"/>"</xsl:when>
		<xsl:when test="not(@type = 'bool' or @type = 'char*' or @type='string-list'  or @type='string-set'   or @type='enum-set') and @default">. Default: "<xsl:value-of select="@default"/>"</xsl:when>
		<xsl:otherwise></xsl:otherwise>
</xsl:choose>

<xsl:text>
</xsl:text>
</xsl:for-each>


#Author

Pierre Lindenbaum PhD


</xsl:template>


</xsl:stylesheet>
