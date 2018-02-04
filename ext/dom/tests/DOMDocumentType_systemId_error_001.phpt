--TEST--
DOMDocumentType::systemId with invalid state.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-25
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
$doctype = new DOMDocumentType();
$systemId = $doctype->systemId;
?>
--EXPECTF--
Warning: main(): Invalid State Error in %s on line %d