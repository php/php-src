--TEST--
DomDocument::CreateEntityReference() - Empty Arguments
--CREDITS--
Clint Priest @ PhpTek09
--FILE--
<?php
	$objDoc = new DomDocument();
	
	$objRef = $objDoc->createEntityReference();
?>
--EXPECTF--
Warning: DOMDocument::createEntityReference() expects exactly 1 parameter, 0 given in %s