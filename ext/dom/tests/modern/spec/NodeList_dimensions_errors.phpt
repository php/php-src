--TEST--
NodeList dimensions errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root><a/><b/><c/></root>');
$children = $dom->documentElement->childNodes;

try {
    $children[][0] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot append to DOM\NodeList
