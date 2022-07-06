<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" encoding="UTF-8"/>

	<xsl:key name="area" match="ROW" use="substring(translate(AREA_NAME, '&quot;', ''), 1, 1)"/>
<xsl:template match="*">
    HERE
</xsl:template>
</xsl:stylesheet>
