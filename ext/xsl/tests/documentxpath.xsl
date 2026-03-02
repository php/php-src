<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >
    <xsl:output  method="xml" encoding="iso-8859-1" indent="no"/>
<xsl:template match="/">

<xsl:value-of select="document('compress.zlib://ext/xsl/tests/xslt.xsl.gz')/xsl:stylesheet/xsl:param/@name"/>
</xsl:template>
    
 </xsl:stylesheet>
