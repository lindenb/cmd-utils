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
#include &lt;set&gt;
#include "throw.hh"
#include "abstractapp.hh"




class <xsl:value-of select="@name"/>Base : public AbstractApplication
	{
	public:
		<xsl:for-each select="options/option[@type='enum' or @type='enum-set']">
		
		enum E_<xsl:value-of select="@name"/>_T
			{
			<xsl:for-each select="item">
				<xsl:if test="position()&gt;1">,</xsl:if>
				E_<xsl:value-of select="../@name"/>_<xsl:value-of select="@name"/>=<xsl:value-of select="count(preceding-sibling::item)"/>
			</xsl:for-each>
			};
		
		int  find_<xsl:value-of select="@name"/>_by_name(const char* s)
			{
			if(s == NULL) {
				return -1;
				}
			<xsl:for-each select="item"> else if (std::strcmp(s,"<xsl:value-of select="@name"/>")==0 ) {
				return E_<xsl:value-of select="../@name"/>_<xsl:value-of select="@name"/>;
				} </xsl:for-each>
			else
				{
				return -1;
				}
			}
		
		const char*  <xsl:value-of select="@name"/>_to_string(int e)
			{
			switch(e)
				{
				<xsl:for-each select="item">
				case E_<xsl:value-of select="../@name"/>_<xsl:value-of select="@name"/> :
					return "<xsl:value-of select="@name"/>";
					break;
				</xsl:for-each>
				default : return NULL ; break;
				}
			}
		
		
		</xsl:for-each>
	
	
	
	
		<xsl:for-each select="options/option">
			<xsl:choose>
				<xsl:when test="@type = 'bool'">bool <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'string-list'">std::vector&lt;std::string&gt; <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'string-set'">std::set&lt;std::string&gt; <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'string'">std::string <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'char*'">char* <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'char'">char <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'int'">int <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'long'">int <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'double'">double <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'enum'">int <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:when test="@type = 'enum-set'">std::set&lt;int&gt; <xsl:value-of select="@name"/>;
				</xsl:when>
				<xsl:otherwise><xsl:message terminate="yes">undefined type</xsl:message></xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
		<xsl:value-of select="@name"/>Base()
			<xsl:if test="options/option">
			<xsl:text>:</xsl:text>
				<xsl:for-each select="options/option">
				<xsl:if test="position()&gt;1">,</xsl:if>
				<xsl:value-of select="@name"/>
				<xsl:text>(</xsl:text>
				<xsl:choose>
					<xsl:when test="@type = 'bool'">false</xsl:when>
					<xsl:when test="@type = 'string-list'"></xsl:when>
					<xsl:when test="@type = 'string-set'"></xsl:when>
					<xsl:when test="@type = 'enum-set'"></xsl:when>
					<xsl:when test="@type = 'enum'">
						<xsl:choose>
							<xsl:when test="@default">E_<xsl:value-of select="@name"/>_<xsl:value-of select="@default"/></xsl:when>
							<xsl:otherwise>-1</xsl:otherwise>
						</xsl:choose>
					</xsl:when>
					<xsl:when test="@type = 'char*'">
						<xsl:choose>
							<xsl:when test="@default"><xsl:message terminate="yes">char* cannot have a default value</xsl:message></xsl:when>
							<xsl:otherwise>NULL</xsl:otherwise>
						</xsl:choose>
					</xsl:when>
					<xsl:when test="@type = 'string'">
						<xsl:choose>
							<xsl:when test="@default">"<xsl:value-of select="@default"/>"</xsl:when>
							<xsl:otherwise>""</xsl:otherwise>
						</xsl:choose>
					</xsl:when>
					<xsl:when test="@type = 'char'">
						<xsl:choose>
							<xsl:when test="@default">'<xsl:value-of select="@default"/>'</xsl:when>
							<xsl:otherwise>'\0'</xsl:otherwise>
						</xsl:choose>
					</xsl:when>
					<xsl:when test="@type = 'int'">
						<xsl:choose>
							<xsl:when test="@default"><xsl:value-of select="@default"/></xsl:when>
							<xsl:otherwise>0</xsl:otherwise>
						</xsl:choose>
					</xsl:when>
					<xsl:when test="@type = 'long'">
						<xsl:choose>
							<xsl:when test="@default"><xsl:value-of select="@default"/></xsl:when>
							<xsl:otherwise>0L</xsl:otherwise>
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
				</xsl:for-each>
			</xsl:if> {

		}
	virtual ~<xsl:value-of select="@name"/>Base() {
		}
		
	virtual const char* get_application_name() const {
		return "<xsl:value-of select="@name"/>";
		}
	
	virtual void usage(std::ostream&amp; out) {
		out &lt;&lt; this-&gt;get_application_name() &lt;&lt; std::endl;
		out &lt;&lt; "Compilation: " &lt;&lt; __DATE__ &lt;&lt; std::endl;
		out &lt;&lt; "Options:" &lt;&lt; std::endl;
		out &lt;&lt; "  -h|--help this screen" &lt;&lt; std::endl;
		out &lt;&lt; "  --version print version and exit." &lt;&lt; std::endl;
		<xsl:for-each select="options/option">out &lt;&lt; "  <xsl:if test="@opt">-<xsl:value-of select="@opt"/></xsl:if><xsl:if test="@opt and @longopt">|</xsl:if><xsl:if test="@longopt">--<xsl:value-of select="@longopt"/></xsl:if><xsl:text>   </xsl:text><xsl:apply-templates select="description"/><xsl:choose>
		<xsl:when test="@type='enum'">. Possible values: [<xsl:for-each select="item"><xsl:if test="position()&gt;1">|</xsl:if><xsl:value-of select="@name"/></xsl:for-each>]. Default: \"" &lt;&lt; <xsl:value-of select="@name"/>_to_string(this-&gt;<xsl:value-of select="@name"/>) &lt;&lt; "\"</xsl:when>
		<xsl:when test="not(@type = 'bool' or @type = 'char*' or @type='string-list'  or @type='string-set'   or @type='enum-set') and @default">. Default: \"" &lt;&lt; this-><xsl:value-of select="@name"/> &lt;&lt;  "\" </xsl:when>
		<xsl:otherwise></xsl:otherwise></xsl:choose>."  &lt;&lt; std::endl;
		</xsl:for-each>
		out &lt;&lt; std::endl;
		}
	
	virtual int parseOptions(int argc, char **argv)
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
					<xsl:otherwise><xsl:value-of select="259+  count(preceding-sibling::option)"/></xsl:otherwise>
				</xsl:choose>
				<xsl:text>},
				</xsl:text>
				</xsl:if></xsl:for-each>
				  {"help",no_argument,0,'h'},
				  {"version",no_argument,0,258},
				  {0, 0, 0, 0}
				};

			  int option_index = 0;
			  int c = ::getopt_long (argc, argv, "h<xsl:apply-templates select="options/option" mode="optstr"/>",long_options, &amp;option_index);

			  if (c == -1) break;

			  switch (c)
				{
				case 'h':
					usage(std::cout);
					exit(EXIT_SUCCESS);
					break;
				case 258:
					std::cout &lt;&lt; <xsl:choose>
						<xsl:when test="@version">"<xsl:value-of select="@version"/>"</xsl:when>
						<xsl:otherwise>"1.0"</xsl:otherwise>
						</xsl:choose> &lt;&lt; std::endl ;
					exit(EXIT_SUCCESS);
					break;
				<xsl:for-each  select="options/option">
				case <xsl:choose>
					<xsl:when test='not(@opt)'><xsl:value-of select="259 +  count(preceding-sibling::option)"/></xsl:when>
					<xsl:otherwise>'<xsl:value-of select="@opt"/>'</xsl:otherwise>
					</xsl:choose> 
					<xsl:text> : </xsl:text>
					<xsl:if test="not(@type = 'bool')">
					if( optarg == NULL) {
						std::cerr &lt;&lt; "Argument is missing for option <xsl:apply-templates select="." mode="label"/>." &lt;&lt; std::endl;
						std::exit(EXIT_FAILURE);
						}
					
					</xsl:if>
					
					
					<xsl:choose>
					<xsl:when test="@type = 'bool'"> this-&gt;<xsl:value-of select="@name"/> = true; </xsl:when>
					<xsl:when test="@type = 'string'"> this-&gt;<xsl:value-of select="@name"/>.assign(optarg); </xsl:when>
					<xsl:when test="@type = 'char*'"> this-&gt;<xsl:value-of select="@name"/> = optarg; </xsl:when>
					<xsl:when test="@type = 'enum'"> this-&gt;<xsl:value-of select="@name"/> = find_<xsl:value-of select="@name"/>_by_name( optarg); 
					if(  this-&gt;<xsl:value-of select="@name"/> == -1 ) {
						std::cerr &lt;&lt; "invalid enum-item in option <xsl:apply-templates select="." mode="label"/>." &lt;&lt; std::endl;
						std::exit(EXIT_FAILURE);
					}
					</xsl:when>
					
					<xsl:when test="@type = 'enum-set'">{
					int <xsl:value-of select="generate-id(.)"/> = find_<xsl:value-of select="@name"/>_by_name( optarg); 
					if(  <xsl:value-of select="generate-id(.)"/> == -1 ) {
						std::cerr &lt;&lt; "invalid enum-item in option <xsl:apply-templates select="." mode="label"/>." &lt;&lt; std::endl;
						std::exit(EXIT_FAILURE);
					}
					this-&gt;<xsl:value-of select="@name"/>.insert(<xsl:value-of select="generate-id(.)"/>);					
					
					break;
					}
					</xsl:when>
					
					<xsl:when test="@type = 'char'"> if(std::strcmp(optarg,"\\t")==0) {
						this-&gt;<xsl:value-of select="@name"/> = '\t';
						}
					else if(std::strcmp(optarg,"\\n")==0) {
						this-&gt;<xsl:value-of select="@name"/> = '\n';
						}
					else if(std::strcmp(optarg,"\\0")==0) {
						this-&gt;<xsl:value-of select="@name"/> = '\0';
						} 
					else if(std::strlen(optarg)!=1)
						{
						std::cerr &lt;&lt; "Option <xsl:apply-templates select="." mode="label"/> expect a character but got input with length != 1." &lt;&lt; std::endl;
						std::exit(EXIT_FAILURE); 
						}
					else
						{
						this-&gt;<xsl:value-of select="@name"/> = optarg[0];
						}
					</xsl:when>
					<xsl:when test="@type = 'string-list'"> this-&gt;<xsl:value-of select="@name"/>.push_back(optarg); </xsl:when>
					<xsl:when test="@type = 'string-set'"> this-&gt;<xsl:value-of select="@name"/>.insert(optarg); </xsl:when>
					<xsl:when test="@type = 'int'"> this-&gt;<xsl:value-of select="@name"/>  = atoi(optarg);</xsl:when>
					<xsl:when test="@type = 'long'"> {
					char* endptr = 0;
					this-&gt;<xsl:value-of select="@name"/>  = strtol(optarg,&amp;endptr,10);
					if(*endptr != '\0' || endptr == optarg) {
						std::cerr &lt;&lt; "Option <xsl:apply-templates select="." mode="label"/> expects a long but got \"" &lt;&lt;  optarg &lt;&lt; "\"." &lt;&lt; std::endl;
						std::exit(EXIT_FAILURE); 
						}
					break;
					}</xsl:when>
					<xsl:when test="@type = 'double'"> {
					char* endptr = 0;
					this-&gt;<xsl:value-of select="@name"/>  = strtod(optarg,&amp;endptr);
					if(*endptr != '\0' || endptr == str) {
						std::cerr &lt;&lt; "Option <xsl:apply-templates select="." mode="label"/> expects a double but got \"" &lt;&lt;  optarg &lt;&lt; "\"." &lt;&lt; std::endl;
						std::exit(EXIT_FAILURE); 
						}
					break;
					}</xsl:when>
					<xsl:when test="@type = 'float'"> {
					char* endptr = 0;
					this-&gt;<xsl:value-of select="@name"/>  = strtof(optarg,&amp;endptr);
					if(*endptr != '\0' || endptr == str) {
						std::cerr &lt;&lt; "Option <xsl:apply-templates select="." mode="label"/> expects a float but got \"" &lt;&lt;  optarg &lt;&lt; "\"." &lt;&lt; std::endl;
						std::exit(EXIT_FAILURE); 
						}
					break;
					}</xsl:when>
					<xsl:otherwise><xsl:message terminate="yes">undefined type <xsl:value-of select="@type"/></xsl:message></xsl:otherwise>
					</xsl:choose>
					<xsl:text>break;
				</xsl:text>
				</xsl:for-each>
				case '?':
				  /* getopt_long already printed an error message. */
				  std::exit(EXIT_FAILURE); 
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

<xsl:template match="option" mode="label" >
<xsl:if test="@opt">-<xsl:value-of select="@opt"/></xsl:if>
<xsl:if test="@opt and @longopt">|</xsl:if>
<xsl:if test="@longopt">--<xsl:value-of select="@longopt"/></xsl:if>
</xsl:template>



<xsl:template match="option" mode="optstr" >
<xsl:if test="@opt">
<xsl:if test="@opt='h'"><xsl:message terminate='yes'>option -h is reserved for help</xsl:message></xsl:if>
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
