<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" 	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output encoding="ISO-8859-1" method="text" omit-xml-declaration="yes" />
	<xsl:template match="/qa">
		<xsl:apply-templates select="test" />
	</xsl:template>
	<xsl:template match="test[@type = 'simple']">
		<xsl:value-of select="." />
	</xsl:template>
</xsl:stylesheet>
