--TEST--
GH-21357 (XSLTProcessor works with \DOMDocument, but fails with \Dom\XMLDocument)
--EXTENSIONS--
dom
xsl
--CREDITS--
jacekkow
--FILE--
<?php
$xml = <<<'XML'
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:tns="urn:myns" version="1.0">
	<xsl:template match="tns:referee"/>
</xsl:stylesheet>
XML;

$dom = Dom\XMLDocument::createFromString($xml);
var_dump(new XSLTProcessor()->importStylesheet($dom));
?>
--EXPECT--
bool(true)
