--TEST--
Check xsltprocessor::setparameter error handling with both single and double quotes
--DESCRIPTION--
Memleak: http://bugs.php.net/bug.php?id=48221
--EXTENSIONS--
xsl
--FILE--
<?php
include __DIR__ .'/prepare.inc';
$proc->importStylesheet($xsl);
$proc->setParameter('', '', '"\'');
$proc->transformToXml($dom);
?>
Done
--EXPECT--
Done
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
