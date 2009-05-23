<?xml version="1.0" encoding="iso-8859-1"?>
<!-- $Id: phpfunc-nostring.xsl,v 1.1.2.2 2009-05-23 14:49:55 felipe Exp $ -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:php="http://php.net/xsl"
>
    <xsl:output  method="text" encoding="iso-8859-1" indent="no"/>
    <xsl:template match="/">
     <xsl:value-of select="php:function(123,'this is an example')"/>
    </xsl:template>
</xsl:stylesheet>
