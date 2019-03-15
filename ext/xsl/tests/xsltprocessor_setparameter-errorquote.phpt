--TEST--
Check xsltprocessor::setparameter error handling with both single and double quotes
--DESCRIPTION--
Memleak: http://bugs.php.net/bug.php?id=48221
--SKIPIF--
<?php
        if (!extension_loaded('xsl')) {
                die("skip\n");
        }
?>
--FILE--
<?php
include __DIR__ .'/prepare.inc';
$proc->importStylesheet($xsl);
$proc->setParameter('', '', '"\'');
$proc->transformToXml($dom);
--EXPECTF--
Warning: XSLTProcessor::transformToXml(): Cannot create XPath expression (string contains both quote and double-quotes) in %s on line %d
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
