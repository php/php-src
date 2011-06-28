--TEST--
DOMDocument::loadHTML() should fail if empty string provided as input
--CREDITS--
Knut Urdalen <knut@php.net>
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadHTML('');
?>
--EXPECTF--
Warning: DOMDocument::loadHTML(): Empty string supplied as input in %s on line %d
