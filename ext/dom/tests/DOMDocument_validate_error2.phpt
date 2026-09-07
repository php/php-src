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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Non-static method DOMDocument::validate() cannot be called statically
