--TEST--
Bug #77569 (Write Access Violation in DomImplementation)
--SKIPIF--
<?php
if (!extension_loaded('dom')) die('skip dom extension not available');
?>
--FILE--
<?php
$imp = new DOMImplementation;
$dom = $imp->createDocument("", "");
$dom->encoding = null;
?>
--EXPECTF--
Warning: main(): Invalid Document Encoding in %s on line %d
