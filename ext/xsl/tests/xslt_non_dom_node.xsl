<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:php="http://php.net/xsl"
		xsl:extension-element-prefixes="php"
                version='1.0'>
<xsl:template match="/">
<xsl:value-of select="php:function('nonDomNode')"/>
</xsl:template>
</xsl:stylesheet>
