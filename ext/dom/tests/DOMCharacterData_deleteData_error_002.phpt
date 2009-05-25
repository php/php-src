--TEST--
DOMCharacterData::deleteData() with offset exceeding string size.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
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
$cdata->deleteData(5, 1);
?>
--EXPECTF--
Fatal error: Uncaught exception 'DOMException' with message 'Index Size Error' in %s:%d
Stack trace:
#0 %s(%d): DOMCharacterData->deleteData(5, 1)
#1 {main}
  thrown in %s on line %d