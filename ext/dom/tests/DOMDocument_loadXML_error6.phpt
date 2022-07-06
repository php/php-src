--TEST--
Test DOMDocument::loadXML() with empty file path
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
// create dom document
$dom = new DOMDocument();
try {
    $dom->loadXML("");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
DOMDocument::loadXML(): Argument #1 ($source) must not be empty
