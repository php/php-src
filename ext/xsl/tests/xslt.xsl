<?xml version="1.0" encoding="iso-8859-1"?>
<!-- $Id: xslt.xsl,v 1.2 2003-11-29 13:01:19 chregu Exp $ -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >

    <xsl:output  method="xml" encoding="iso-8859-1" indent="no"/>
    <xsl:param name="foo" select="'bar'"/>
    <xsl:template match="/">
    <html>
    <body>
        <xsl:value-of select="$foo"/><xsl:text>
</xsl:text>
        <xsl:apply-templates select="/chapter/para/informaltable/tgroup/tbody/row"/>
     </body>
     </html>
    </xsl:template>

    <xsl:template match="row">
        <xsl:for-each select="entry">
            <xsl:value-of select="."/>
            <xsl:text> </xsl:text>
        </xsl:for-each>
       <br/> <xsl:text> 
</xsl:text>

    </xsl:template>
</xsl:stylesheet>
