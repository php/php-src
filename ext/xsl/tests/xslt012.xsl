<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >

    <xsl:output  method="xml" encoding="iso-8859-1" indent="no"/>
    <xsl:param name="foo" select="'bar'"/>
    <xsl:param name="foo1" select="'bar1'"/>
    <xsl:template match="/">
    <html>
    <body>
        <xsl:value-of select="$foo"/><xsl:text>
</xsl:text>
        <xsl:value-of select="$foo1"/><xsl:text>
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
