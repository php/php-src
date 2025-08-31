--TEST--
Check xsltprocessor::getParameter with undefined parameter
--EXTENSIONS--
xsl
--FILE--
<?php
include __DIR__ .'/prepare.inc';
var_dump($proc->getParameter('', 'doesnotexist'));
?>
--EXPECT--
bool(false)
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
