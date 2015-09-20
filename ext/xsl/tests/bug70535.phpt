--TEST--
Bug #70535 (XSLT: free(): invalid pointer)
--SKIPIF--
<?php
if (!extension_loaded('xsl')) die("skip Extension XSL is required\n");
if (!extension_loaded("simplexml")) print "skip simplexml extension is not loaded";
?>
--FILE--
<?php

$xmlInput = simplexml_load_string('<root></root>');
$xslInput = '<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:template match="root"><xsl:text>success</xsl:text></xsl:template></xsl:stylesheet>';

$xsl = new \DomDocument();
$xsl->loadXML($xslInput);
$xslt = new \XsltProcessor();
$xslt->importStylesheet($xsl);
$xmloutput = $xslt->transformToXml($xmlInput);
?>
okey
--EXPECT--
okey
