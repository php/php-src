<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:php="http://php.net/xsl"
		xsl:extension-element-prefixes="php"
                version='1.0'>
<xsl:template match="/">
<xsl:value-of select="php:functionString('foobar', /doc/@id, 'secondArg')"/>
<xsl:text>
</xsl:text>
<xsl:value-of select="php:function('foobar', /doc/@id)"/>
<xsl:text>
</xsl:text>
<xsl:value-of select="php:function('nodeSet')"/>
<xsl:text>
</xsl:text>
<xsl:value-of select="php:function('nodeSet',/doc)/i"/>
<xsl:text>
</xsl:text>
<xsl:value-of select="php:function('aClass::aStaticFunction','static')"/>
<xsl:text>
</xsl:text>
</xsl:template>
</xsl:stylesheet>
