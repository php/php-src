--TEST--
DomDocument::CreateEntityReference() - Empty Arguments
--CREDITS--
Clint Priest @ PhpTek09
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
	$objDoc = new DomDocument();
	
	$objRef = $objDoc->createEntityReference();
?>
===DONE===
--EXPECTF--
Warning: DOMDocument::createEntityReference() expects exactly 1 parameter, 0 given in %s
===DONE===
	