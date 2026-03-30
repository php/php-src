--TEST--
Special operations with XSLTProcessor properties
--EXTENSIONS--
xsl
dom
--FILE--
<?php

$xml = new DOMDocument;
$xml->loadXML('<?xml version="1.0"?><root><foo>hello</foo></root>');

function test() {
    echo "Called test\n";
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

echo "--- Unset cloneDocument ---\n";

$xslt = new XSLTProcessor;
$xslt->registerPHPFunctions();
unset($xslt->cloneDocument);
$xslt->importStylesheet($xsl);
echo $xslt->transformToXml($xml);

echo "--- Unset doXInclude ---\n";

$xslt = new XSLTProcessor;
$xslt->registerPHPFunctions();
unset($xslt->doXInclude);
$xslt->importStylesheet($xsl);
echo $xslt->transformToXml($xml);

echo "--- Make properties references ---\n";

$xslt = new XSLTProcessor;
$xslt->registerPHPFunctions();
$clone =& $xslt->cloneDocument;
$xinclude =& $xslt->doXInclude;
$xslt->importStylesheet($xsl);
echo $xslt->transformToXml($xml);

?>
--EXPECT--
--- Unset cloneDocument ---
Called test
<?xml version="1.0"?>
hello
--- Unset doXInclude ---
Called test
<?xml version="1.0"?>
hello
--- Make properties references ---
Called test
<?xml version="1.0"?>
hello
