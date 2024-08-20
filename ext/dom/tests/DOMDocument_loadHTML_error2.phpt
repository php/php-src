--TEST--
DOMDocument::loadHTML() should fail if empty string provided as input
--CREDITS--
Knut Urdalen <knut@php.net>
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
try {
    $doc->loadHTML('');
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
DOMDocument::loadHTML(): Argument #1 ($source) must not be empty
