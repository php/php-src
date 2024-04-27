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
try {
    $xslt->importStylesheet($xsl);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Unset doXInclude ---\n";

$xslt = new XSLTProcessor;
$xslt->registerPHPFunctions();
unset($xslt->doXInclude);
$xslt->importStylesheet($xsl);
try {
    echo $xslt->transformToXml($xml);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

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
Typed property XSLTProcessor::$cloneDocument must not be accessed before initialization
--- Unset doXInclude ---
Typed property XSLTProcessor::$doXInclude must not be accessed before initialization
--- Make properties references ---
Called test
<?xml version="1.0"?>
hello
