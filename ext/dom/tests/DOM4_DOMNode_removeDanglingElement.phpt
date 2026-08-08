--TEST--
DOMNode::remove() dangling element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();

$element = $dom->createElement('test');

try {
    $element->remove();
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
DOMException: Not Found Error
