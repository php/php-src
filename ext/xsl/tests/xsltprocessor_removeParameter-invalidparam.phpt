--TEST--
Check xsltprocessor::removeParameter with invalid parameter
--EXTENSIONS--
xsl
--FILE--
<?php
include __DIR__ .'/prepare.inc';
$proc->importStylesheet($xsl);
var_dump($proc->removeParameter('', 'doesnotexist'));
?>
--EXPECT--
bool(false)
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
