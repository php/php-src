<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" omit-xml-declaration="yes" encoding="ISO-8859-1" />
	<xsl:param name="insertion">Test failed</xsl:param>
	<xsl:template match="/qa">
		<xsl:apply-templates select="test" />
	</xsl:template>
	<xsl:template match="test">
		<xsl:choose>
			<xsl:when test="@type != 'simple'">
				<xsl:value-of select="$insertion" />
			</xsl:when>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>
