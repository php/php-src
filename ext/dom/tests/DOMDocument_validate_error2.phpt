--TEST--
DOMDocument::validate() should fail if called statically
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--EXTENSIONS--
dom
--FILE--
<?php
try {
    DOMDocument::validate();
} catch (Error $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Non-static method DOMDocument::validate() cannot be called statically
