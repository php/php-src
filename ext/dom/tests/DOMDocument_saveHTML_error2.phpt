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
Fatal error: Uncaught exception 'Error' with message 'Non-static method DOMDocument::saveHTML() cannot be called statically' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
