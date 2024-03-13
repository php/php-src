--TEST--
TokenList: add errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString("<root/>");
$list = $dom->documentElement->classList;

try {
    $list->add("");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $list->add("  ");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $list->add("\0");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $list->add(0);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo $dom->saveXML(), "\n";

?>
--EXPECT--
The empty string is not a valid token
The token must not contain any ASCII whitespace
DOM\TokenList::add(): Argument #1 must not contain any null bytes
DOM\TokenList::add(): Argument #1 must be of type string, int given
<?xml version="1.0" encoding="UTF-8"?>
<root/>
