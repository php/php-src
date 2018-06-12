--TEST--
DomDocument::CreateComment() - Incorrect Parameters
--CREDITS--
Clint Priest @ PhpTek09
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
	$x = new DomDocument();
	$x->createComment();
?>
===DONE===
--EXPECTF--
Warning: DOMDocument::createComment() expects exactly 1 parameter, 0 given in %s
===DONE===
	