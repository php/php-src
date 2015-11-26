--TEST--
DOMDocumentFragment::appendXML() with no arguments.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$fragment = new DOMDocumentFragment();
$fragment->appendXML();
?>
--EXPECTF--
Warning: DOMDocumentFragment::appendXML() expects exactly 1 parameter, 0 given in %s on line %d