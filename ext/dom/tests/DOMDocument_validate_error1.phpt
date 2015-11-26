--TEST--
DOMDocument::validate() should fail if any parameter is given
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php
require_once dirname(__FILE__) .'/skipif.inc';
?>
--FILE--
<?php
$dom = new DOMDocument('1.0');
$dom->validate(true);
?>
--EXPECTF--
Warning: DOMDocument::validate() expects exactly 0 parameters, 1 given in %s on line %d
