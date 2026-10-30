--TEST--
cloneDocument
--EXTENSIONS--
xsl
dom
--FILE--
<?php

$xml = new DOMDocument;
$xml->loadXML('<?xml version="1.0"?><root><foo>hello</foo></root>');

function test() {
    global $xml;
    $xml->documentElement->firstChild->textContent = "bye";
}

$xsl = new DOMDocument;
$xsl->loadXML(<<<XML
<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl" version="1.0">
    <xsl:template match="/root">
        <xsl:value-of select="php:function('test')"/>
        <xsl:value-of select="//root/foo"/>
    </xsl:template>
</xsl:stylesheet>
XML);

$xslt = new XSLTProcessor;
$xslt->registerPHPFunctions();
$xslt->cloneDocument = true;
$xslt->importStylesheet($xsl);
echo $xslt->transformToXml($xml);

$xslt = new XSLTProcessor;
$xslt->registerPHPFunctions();
$xslt->cloneDocument = false;
$xslt->importStylesheet($xsl);
echo $xslt->transformToXml($xml);

?>
--EXPECT--
<?xml version="1.0"?>
hello
<?xml version="1.0"?>
bye
