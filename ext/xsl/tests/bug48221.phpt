--TEST--
Bug #48221 (memory leak when passing invalid xslt parameter)
--EXTENSIONS--
xsl
--FILE--
<?php
include('prepare.inc');
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
