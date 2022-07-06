--TEST--
Bug #48221 (memory leak when passing invalid xslt parameter)
--SKIPIF--
<?php
if (!extension_loaded('xsl')) die("skip Extension XSL is required\n");
?>
--FILE--
<?php
include('prepare.inc');
$proc->importStylesheet($xsl);
$proc->setParameter('', '', '"\'');
$proc->transformToXml($dom);
?>
--EXPECTF--
Warning: XSLTProcessor::transformToXml(): Cannot create XPath expression (string contains both quote and double-quotes) in %s on line %d
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
