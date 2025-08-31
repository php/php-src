--TEST--
Ensure that Dom\Node::$prefix is read-only
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();
$div = $dom->createElement('div');
try {
    $div->prefix = "foo";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveXml();
?>
--EXPECT--
Cannot modify readonly property Dom\HTMLElement::$prefix
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
