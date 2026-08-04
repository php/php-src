--TEST--
GH-21357 (XSLTProcessor works with \DOMDocument, but fails with \Dom\XMLDocument)
--EXTENSIONS--
dom
xsl
--CREDITS--
jacekkow
--FILE--
<?php
$xsl = '<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
    version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:old="http://something/old/"
    xmlns:new="http://something/new/">
  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
  </xsl:template>
  <xsl:template match="old:*">
    <xsl:element name="{local-name()}" xmlns="http://something/new/" >
      <xsl:apply-templates select="node()|@*"/>
    </xsl:element>
  </xsl:template>
</xsl:stylesheet>';
$dom = Dom\XMLDocument::createFromString($xsl);
$xsl = new XSLTProcessor();
$xsl->importStylesheet($dom);
var_dump($xsl->transformToXml(\Dom\XMLDocument::createFromString('<test xmlns="http://something/old/"/>')));
?>
--EXPECT--
string(138) "<?xml version="1.0"?>
<test xmlns="http://something/new/" xmlns:ns_1="http://www.w3.org/2000/xmlns/" ns_1:xmlns="http://something/old/"/>
"
