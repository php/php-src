--TEST--
XSLTProcessor refuses to transform a document that an XPath evaluation is using
--EXTENSIONS--
dom
xsl
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<root><a>x</a> <b>y</b> <c>z</c></root>');

$xsl = new DOMDocument();
$xsl->loadXML('<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:strip-space elements="*"/><xsl:output method="text"/><xsl:template match="/"><xsl:value-of select="count(/root/node())"/></xsl:template></xsl:stylesheet>');

$proc = new XSLTProcessor();
$proc->importStylesheet($xsl);

$cloning = new XSLTProcessor();
$cloning->cloneDocument = true;
$cloning->importStylesheet($xsl);

function transform() {
    static $called = false;
    if (!$called) {
        $called = true;
        try {
            var_dump($GLOBALS['proc']->transformToXml($GLOBALS['doc']));
        } catch (DOMException $e) {
            echo $e::class, ": ", $e->getMessage(), PHP_EOL;
        }
        var_dump($GLOBALS['cloning']->transformToXml($GLOBALS['doc']));
    }
    return true;
}

$xpath = new DOMXPath($doc);
$xpath->registerNamespace('php', 'http://php.net/xpath');
$xpath->registerPhpFunctions('transform');
var_dump($xpath->query('/root/node()[php:function("transform")]')->length);
var_dump($proc->transformToXml($doc));

?>
--EXPECT--
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
string(1) "3"
int(5)
string(1) "3"
