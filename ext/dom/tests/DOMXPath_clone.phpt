--TEST--
DOMXPath: Cloning a DOMXPath object
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (!class_exists('DOMXPath')) die('skip no xpath support');
?>
--FILE--
<?php

$dom = new DOMDocument;
$xpath = new DOMXPath($dom);
try {
    clone $xpath;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Trying to clone an uncloneable object of class DOMXPath
