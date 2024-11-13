--TEST--
Test: Xinclude errors
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip for 64bit platforms only");
?>
--FILE--
<?php
$dom = new DOMDocument;
$dom->xinclude(PHP_INT_MAX);

$dom = Dom\XMLDocument::createEmpty();
try {
    $dom->xinclude(PHP_INT_MAX);
} catch (ValueError $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
Warning: DOMDocument::xinclude(): Invalid flags in %s on line %d
Dom\XMLDocument::xinclude(): Argument #1 ($options) is too large
