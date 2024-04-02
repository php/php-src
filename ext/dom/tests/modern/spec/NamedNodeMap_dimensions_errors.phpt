--TEST--
NamedNodeMap dimensions errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root a="1" b="2" c="3"></root>');
$attributes = $dom->documentElement->attributes;

try {
    $attributes[][0] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot append to DOM\NamedNodeMap
