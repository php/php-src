--TEST--
DomDocument::CreateComment() - Incorrect Parameters
--CREDITS--
Clint Priest @ PhpTek09
--FILE--
<?php
	$x = new DomDocument();
	$x->createComment();
?>
--EXPECTF--
Warning: DOMDocument::createComment() expects exactly 1 parameter, 0 given in %s