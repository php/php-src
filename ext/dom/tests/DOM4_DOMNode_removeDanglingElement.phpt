--TEST--
DOMNode::remove() dangling element
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$dom = new DOMDocument();

$element = $dom->createElement('test');

try {
    $element->remove();
} catch (DOMException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Not Found Error
