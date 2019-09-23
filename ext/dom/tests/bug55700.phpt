--TEST--
Bug #55700 (XPath namespace prefix conflict, global registerNodeNS flag)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<prefix:root xmlns:prefix="urn:a" />');

$xp = new DOMXPath($doc, true);
$xp->registerNamespace('prefix', 'urn:b');

echo($xp->query('//prefix:root')->length . "\n");

$xp = new DOMXPath($doc, false);
$xp->registerNamespace('prefix', 'urn:b');

echo($xp->query('//prefix:root')->length . "\n");

?>
--EXPECT--
1
0
