--TEST--
Bug #49490 (XPath namespace prefix conflict).
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<prefix:root xmlns:prefix="urn:a" />');

$xp = new DOMXPath($doc);
$xp->registerNamespace('prefix', 'urn:b');

echo($xp->query('//prefix:root', null, false)->length . "\n");

?>
--EXPECT--
0
