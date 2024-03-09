--TEST--
Ensure that DOM\Node::$prefix is read-only
--EXTENSIONS--
dom
--FILE--
<?php
$dom = DOM\HTMLDocument::createEmpty();
$div = $dom->createElement('div');
try {
    $div->prefix = "foo";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveXML();
?>
--EXPECT--
Cannot modify readonly property DOM\Element::$prefix
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
