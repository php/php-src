--TEST--
Bug #37456 (DOMElement->setAttribute() loops forever)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument();
$doc->resolveExternals = true;
$doc->load(__DIR__."/dom.xml");

$root = $doc->getElementsByTagName('foo')->item(0);
$root->setAttribute('bar', '&gt;');
$attr = $root->setAttribute('bar', 'newval');
print $attr->nodeValue;


?>
--EXPECT--
newval
