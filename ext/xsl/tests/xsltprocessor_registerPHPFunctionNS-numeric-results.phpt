--TEST--
Returning numeric values from XSLTProcessor callbacks
--EXTENSIONS--
xsl
--FILE--
<?php
$document = Dom\XMLDocument::createFromString('<root><a/><b/><c/><d/></root>');
$xslt =<<<END
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0" xmlns:my="urn:my.ns">
<xsl:output method="text" omit-xml-declaration="yes"/>
<xsl:template match="/root">count: <xsl:value-of select="my:count(*)"/></xsl:template>
</xsl:stylesheet>
END;
$stylesheet = Dom\XMLDocument::createFromString($xslt);
$xsltProcessor = new XSLTProcessor;
$xsltProcessor->importStylesheet($stylesheet);
$xsltProcessor->registerPHPFunctionNS('urn:my.ns', 'count', fn(array $arg1) => count($arg1));
var_dump($xsltProcessor->transformToXML($document));
?>
--EXPECT--
string(8) "count: 4"
