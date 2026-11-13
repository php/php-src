--TEST--
XSLTProcessor refuses a nested transformation on the same processor
--EXTENSIONS--
dom
xsl
--FILE--
<?php

function nested($nodes) {
    static $called = false;
    if ($called) {
        return 'v';
    }
    $called = true;
    $proc = $GLOBALS['proc'];
    $doc = $GLOBALS['doc'];
    try {
        var_dump($proc->transformToXml($doc));
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
    try {
        var_dump($proc->transformToDoc($doc));
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
    try {
        var_dump($proc->transformToUri($doc, $GLOBALS['uri']));
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
    return 'v';
}

function suspend($nodes) {
    Fiber::suspend();
    return 'f';
}

function text($nodes) {
    return $nodes[0]->textContent;
}

$doc = new DOMDocument();
$doc->loadXML('<root><a>1</a><b>2</b><c>3</c></root>');

$xsl = new DOMDocument();
$xsl->loadXML(<<<XML
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl">
  <xsl:key name="k" match="*" use="name()"/>
  <xsl:param name="cb" select="'nested'"/>
  <xsl:template match="/"><xsl:for-each select="/root/*">[<xsl:value-of select="php:function(\$cb, .)"/><xsl:value-of select="name(key('k', name(.)))"/>]</xsl:for-each></xsl:template>
</xsl:stylesheet>
XML);

$uri = __DIR__ . '/xsltprocessor_nested_transform.xml';

$proc = new XSLTProcessor();
$proc->registerPHPFunctions();
$proc->importStylesheet($xsl);

var_dump($proc->transformToXml($doc));

$proc->setParameter('', 'cb', 'suspend');
$fiber = new Fiber(fn () => $proc->transformToXml($doc));
$fiber->start();
try {
    var_dump($proc->transformToXml($doc));
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
while (!$fiber->isTerminated()) {
    $fiber->resume();
}
var_dump($fiber->getReturn());

$proc->setParameter('', 'cb', 'text');
var_dump($proc->transformToXml($doc));
var_dump($proc->transformToDoc($doc)->textContent);

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/xsltprocessor_nested_transform.xml');
?>
--EXPECT--
Error: Cannot call XSLTProcessor::transformToXml() while a transformation is in progress
Error: Cannot call XSLTProcessor::transformToDoc() while a transformation is in progress
Error: Cannot call XSLTProcessor::transformToUri() while a transformation is in progress
string(35) "<?xml version="1.0"?>
[va][vb][vc]
"
Error: Cannot call XSLTProcessor::transformToXml() while a transformation is in progress
string(35) "<?xml version="1.0"?>
[fa][fb][fc]
"
string(35) "<?xml version="1.0"?>
[1a][2b][3c]
"
string(12) "[1a][2b][3c]"
