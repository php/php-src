--TEST--
TokenList: remove errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString("<root/>");
$list = $dom->documentElement->classList;

try {
    $list->remove("");
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $list->remove("  ");
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $list->remove("\0");
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $list->remove(0);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo $dom->saveXML(), "\n";

?>
--EXPECT--
DOMException: The empty string is not a valid token
DOMException: The token must not contain any ASCII whitespace
ValueError: Dom\TokenList::remove(): Argument #1 must not contain any null bytes
TypeError: Dom\TokenList::remove(): Argument #1 must be of type string, int given
<?xml version="1.0" encoding="UTF-8"?>
<root/>
