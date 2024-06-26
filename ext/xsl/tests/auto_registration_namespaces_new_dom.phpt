--TEST--
Auto-registration of namespaces in XSL stylesheet with new DOM
--EXTENSIONS--
dom
xsl
--SKIPIF--
<?php
require __DIR__.'/skip_upstream_issue113.inc';
?>
--FILE--
<?php

$sheet = Dom\XMLDocument::createFromString(<<<XML
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:php="http://php.net/xsl"
    xmlns:str="http://exslt.org/strings"
    xmlns:xsdl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="text" omit-xml-declaration="yes" indent="no"/>
    <xsl:template match="/root">
        <xsl:value-of select="php:function('strtoupper', string(./hello))"/>
        <xsl:value-of select="test:reverse(string(./hello))"/>
    </xsl:template>
</xsl:stylesheet>
XML);

// Make sure it will auto-register urn:test
$sheet->documentElement->append($sheet->createElementNS('urn:test', 'test:dummy'));

$input = Dom\XMLDocument::createFromString(<<<XML
<root>
    <hello>World</hello>
</root>
XML);

$processor = new XSLTProcessor();
$processor->importStylesheet($sheet);
$processor->registerPHPFunctions();
$processor->registerPHPFunctionNS('urn:test', 'reverse', 'strrev');
echo $processor->transformToXML($input), "\n";

?>
--EXPECT--
WORLDdlroW
