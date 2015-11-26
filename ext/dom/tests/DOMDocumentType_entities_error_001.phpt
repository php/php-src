--TEST--
DOMDocumentType::entities with invalid state.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-25
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$doctype = new DOMDocumentType();
$entities = $doctype->entities;
?>
--EXPECTF--
Warning: main(): Invalid State Error in %s on line %d