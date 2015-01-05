--TEST--
DOMDocument::saveHTML() should fail if called statically
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php
require_once dirname(__FILE__) .'/skipif.inc';
?>
--FILE--
<?php
DOMDocument::saveHTML(true);
?>
--EXPECTF--
Fatal error: Non-static method DOMDocument::saveHTML() cannot be called statically in %s on line %d
