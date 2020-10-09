--TEST--
DOMDocument::saveHTML() should fail if called statically
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php
require_once __DIR__ .'/skipif.inc';
?>
--FILE--
<?php
try {
    DOMDocument::saveHTML(true);
} catch (Error $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Non-static method DOMDocument::saveHTML() cannot be called statically
