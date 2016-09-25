<?xml version='1.0'  encoding="ISO-8859-1"?>
<xsl:stylesheet
	xmlns:xsl='http://www.w3.org/1999/XSL/Transform'
	xmlns:time="http://exslt.org/dates-and-times"
	version='1.1'
	>
<xsl:output method='text' encoding="UTF-8"/>

<xsl:variable name="lower">abcdefghijklmnopqrstuvwxyz</xsl:variable>
<xsl:variable name="upper">ABCDEFGHIJKLMNOPQRSTUVWXYZ</xsl:variable>

<xsl:template match="cmd" >
<xsl:variable name="incl" select="concat(translate(@name,$lower,$upper),'_',generate-id(.),'_HH')"/>
#ifndef <xsl:value-of select="$incl"/>
#define <xsl:value-of select="$incl"/>
#include &lt;iostream&gt;
#include &lt;cstdio&gt;
#include &lt;cstdlib&gt;
#include &lt;cstring&gt;
#include &lt;getopt.h&gt;
#include &lt;string&gt;
#include &lt;vector&gt;




class <xsl:value-of select="@name"/>Base
	{
	public:
		<xsl:for-each select="enum">
		class <xsl:value-of select="@name"/>
			{
			private:
				const char* items[<xsl:value-of select="count(item)"/>];
			public:
				typedef int type;
				<xsl:value-of select="@name"/>()
					{
					<xsl:for-each select="item"> this->items[<xsl:value-of select="position() - 1"/>]="<xsl:value-of select="@name"/>";
					</xsl:for-each>
					}
				type indexOf(const char* s) {
					for(int i=0;i&lt;<xsl:value-of select="count(item)"/>;++i)
						{
						if(std::strcmp(s,this->items[i])==0) return i;
						}
					return -1;
					}
				const char* toString(type i) const
					{
					if(i&gt;=0 || i&lt;<xsl:value-of select="count(item)"/>)
						{
						return this->items[i];
						}
					return (const char*)0;
					}
				std::ostream&amp; <xsl:value-of select="@name"/>Write(std::ostream&amp; out,const char* delim) const
					{
					for(int i=0;i&lt;<xsl:value-of select="count(item)"/>;++i)
						{
						if(i&gt;0) out &lt;&lt;delim;
						out &lt;&lt; toString(i) ;
						}
					return out;
					}			
			};
		
		
		</xsl:for-each>
	
	
	
	
		<xsl:for-each select="options/option">
			<xsl:choose>
				<xsl:when test="@type = 'bool'">bool <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'string-list'">std::vector&lt;std::string&gt; <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'string'">std::string <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'int'">int <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'double'">double <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:otherwise><xsl:message terminate="yes">undefined type</xsl:message></xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
		<xsl:value-of select="@name"/>Base():<xsl:for-each select="options/option">
			<xsl:if test="position()&gt;1">,</xsl:if>
			<xsl:value-of select="@name"/>
			<xsl:text>(</xsl:text>
			<xsl:choose>
				<xsl:when test="@type = 'bool'">false</xsl:when>
				<xsl:when test="@type = 'string-list'"></xsl:when>
				<xsl:when test="@type = 'string'">
					<xsl:choose>
						<xsl:when test="@default">"<xsl:value-of select="@default"/>"</xsl:when>
						<xsl:otherwise>""</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
				<xsl:when test="@type = 'int'">
					<xsl:choose>
						<xsl:when test="@default"><xsl:value-of select="@default"/></xsl:when>
						<xsl:otherwise>0</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
				<xsl:when test="@type = 'double'">
					<xsl:choose>
						<xsl:when test="@default"><xsl:value-of select="@default"/></xsl:when>
						<xsl:otherwise>0.0</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
				<xsl:otherwise><xsl:message terminate="yes">undefined type</xsl:message></xsl:otherwise>
			</xsl:choose>
			<xsl:text>)</xsl:text>
			</xsl:for-each> {

		}
	void usage(std::ostream&amp; out) {
		out &lt;&lt; "<xsl:value-of select="@name"/>" &lt;&lt; std::endl;
		out &lt;&lt; "Options:" &lt;&lt; std::endl;
		<xsl:for-each select="options/option">out &lt;&lt; "  <xsl:if test="@opt">-<xsl:value-of select="@opt"/></xsl:if><xsl:if test="@opt and @longopt">|</xsl:if><xsl:if test="@longopt">--<xsl:value-of select="@longopt"/></xsl:if><xsl:text>   </xsl:text><xsl:value-of select="@description"/><xsl:if test="not(@type = 'bool' or @type='string-list') and @default">. Default: \"" &lt;&lt; this-><xsl:value-of select="@name"/> &lt;&lt;  "\" </xsl:if>."  &lt;&lt; std::endl;
		</xsl:for-each>
		out &lt;&lt; std::endl;
		}
	
	int parseOptions(int argc, char **argv)
		{
		
		for(;;)
			{
			static struct option long_options[] =
				{
				<xsl:for-each  select="options/option"><xsl:if test="@longopt">
				<xsl:text>{"</xsl:text>
				<xsl:value-of select="@longopt"/>
				<xsl:text>",</xsl:text>
					<xsl:choose>
						<xsl:when test="@type!='bool'">no_argument</xsl:when>
						<xsl:otherwise>required_argument</xsl:otherwise>
					</xsl:choose>
				<xsl:text>,0,</xsl:text>
				<xsl:choose>
					<xsl:when test="@opt">'<xsl:apply-templates select="@opt"/>'</xsl:when>
					<xsl:otherwise><xsl:value-of select="257 +  count(preceding-sibling::option)"/></xsl:otherwise>
				</xsl:choose>
				<xsl:text>},
				</xsl:text>
				</xsl:if></xsl:for-each>
				  {0, 0, 0, 0}
				};

			  int option_index = 0;
			  int c = ::getopt_long (argc, argv, "<xsl:apply-templates select="options/option" mode="optstr"/>",long_options, &amp;option_index);

			  if (c == -1) break;

			  switch (c)
				{
				<xsl:for-each  select="options/option">
				case <xsl:choose>
					<xsl:when test='not(@opt)'><xsl:value-of select="257 +  count(preceding-sibling::option)"/></xsl:when>
					<xsl:otherwise>'<xsl:value-of select="@opt"/>'</xsl:otherwise>
					</xsl:choose> : <xsl:choose>
					<xsl:when test="@type = 'bool'"> this-&gt;<xsl:value-of select="@name"/> = true; </xsl:when>
					<xsl:when test="@type = 'string'"> this-&gt;<xsl:value-of select="@name"/>.assign(optarg); </xsl:when>
					<xsl:when test="@type = 'string-list'"> this-&gt;<xsl:value-of select="@name"/>.push_back(optarg); </xsl:when>
					<xsl:when test="@type = 'int'"> this-&gt;<xsl:value-of select="@name"/>  = atoi(optarg);</xsl:when>
					<xsl:when test="@type = 'long'"> this-&gt;<xsl:value-of select="@name"/>  = atol(optarg);</xsl:when>
					<xsl:when test="@type = 'double'"> this-&gt;<xsl:value-of select="@name"/>  = atod(optarg);</xsl:when>
					<xsl:when test="@type = 'float'"> this-&gt;<xsl:value-of select="@name"/>  = atof(optarg);</xsl:when>
					<xsl:otherwise><xsl:message terminate="yes">undefined type</xsl:message></xsl:otherwise>
					</xsl:choose>
					<xsl:text>break;
				</xsl:text>
				</xsl:for-each>
				
				case '?':
				  /* getopt_long already printed an error message. */
				  break;

				default:
				  abort ();
				}
			}
		return optind;
		}
	
	};


#endif

</xsl:template>




<xsl:template match="option" mode="optstr" >
<xsl:if test="@opt">
<xsl:value-of select="@opt"/>
<xsl:if test="@type!='bool'">:</xsl:if>
</xsl:if>
</xsl:template>


<xsl:template match="option" mode="decl" >
<xsl:if test="@longopt">

</xsl:if>
</xsl:template>

<xsl:template match="option" mode="macro_longopt" >
<xsl:if test="@longopt">
<xsl:value-of select="translate(/cmd/@name,$lower,$upper)"/>
<xsl:text>_OPTION_</xsl:text>
<xsl:apply-templates select="." mode="upper"/>
<xsl:text>_LONGOPT</xsl:text>
</xsl:if>
</xsl:template>

<xsl:template match="option" mode="macro_opt" >
<xsl:if test="@opt">
<xsl:value-of select="translate(/cmd/@name,$lower,$upper)"/>
<xsl:text>_OPTION_</xsl:text>
<xsl:apply-templates select="." mode="upper"/>
<xsl:text>_OPT</xsl:text>
</xsl:if>
</xsl:template>

<xsl:template match="option" mode="upper" >
<xsl:value-of select="translate(@name,$lower,$upper)"/>
</xsl:template>


</xsl:stylesheet>
