--TEST--
Check xsltprocessor::removeParameter functionality
--EXTENSIONS--
xsl
--FILE--
<?php
include __DIR__ .'/prepare.inc';
$proc->importStylesheet($xsl);
$proc->setParameter('', 'key', 'value');
$proc->removeParameter('', 'key');
var_dump($proc->getParameter('', 'key'));
?>
--EXPECT--
bool(false)
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
