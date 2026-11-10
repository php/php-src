--TEST--
XSLTProcessor refuses an XSLT transformation or XInclude on the source document from a php:function() callback
--EXTENSIONS--
dom
xsl
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<root><a>x</a> <b>y</b> <c>z</c></root>');

$strip = new DOMDocument();
$strip->loadXML('<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:strip-space elements="*"/><xsl:output method="text"/><xsl:template match="/"><xsl:value-of select="count(/root/node())"/></xsl:template></xsl:stylesheet>');
$inner = new XSLTProcessor();
$inner->importStylesheet($strip);

function cb() {
    static $called = false;
    if (!$called) {
        $called = true;
        try {
            var_dump($GLOBALS['inner']->transformToXml($GLOBALS['doc']));
        } catch (DOMException $e) {
            echo $e::class, ": ", $e->getMessage(), PHP_EOL;
        }
        try {
            var_dump($GLOBALS['doc']->xinclude());
        } catch (DOMException $e) {
            echo $e::class, ": ", $e->getMessage(), PHP_EOL;
        }
    }
    return '';
}

$xsl = new DOMDocument();
$xsl->loadXML('<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl"><xsl:output method="text"/><xsl:template match="/"><xsl:for-each select="/root/node()"><xsl:value-of select="php:function(\'cb\')"/>[<xsl:value-of select="."/>]</xsl:for-each></xsl:template></xsl:stylesheet>');
$proc = new XSLTProcessor();
$proc->registerPHPFunctions('cb');
$proc->importStylesheet($xsl);
var_dump($proc->transformToXml($doc));
var_dump($inner->transformToXml($doc));

?>
--EXPECT--
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
string(15) "[x][ ][y][ ][z]"
string(1) "3"
