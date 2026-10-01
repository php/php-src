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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
DOMException: Not Found Error
