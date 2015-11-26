--TEST--
DOMDocument::loadHTML() should fail if no parameter is given
--CREDITS--
Knut Urdalen <knut@php.net>
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadHTML();
?>
--EXPECTF--
Warning: DOMDocument::loadHTML() expects at least 1 parameter, 0 given in %s on line %d
