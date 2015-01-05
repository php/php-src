--TEST--
DOMCharacterData::appendData() with no arguments.
--CREDITS--
Eric Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);

$cdata = $document->createCDATASection('test');
$root->appendChild($cdata);
$cdata->appendData();
?>
--EXPECTF--
Warning: DOMCharacterData::appendData() expects exactly 1 parameter, 0 given in %s on line %d