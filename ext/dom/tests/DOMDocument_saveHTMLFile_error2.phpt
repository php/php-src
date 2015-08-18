--TEST--
DOMDocument::saveHTMLFile() should fail if called statically
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php
require_once dirname(__FILE__) .'/skipif.inc';
?>
--FILE--
<?php
DOMDocument::saveHTMLFile();
?>
--EXPECTF--
Fatal error: Uncaught Error: Non-static method DOMDocument::saveHTMLFile() cannot be called statically in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
