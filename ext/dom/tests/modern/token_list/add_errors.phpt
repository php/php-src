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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $list->add("  ");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $list->add("\0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $list->add(0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo $dom->saveXML(), "\n";

?>
--EXPECT--
DOMException: The empty string is not a valid token
DOMException: The token must not contain any ASCII whitespace
ValueError: Dom\TokenList::add(): Argument #1 must not contain any null bytes
TypeError: Dom\TokenList::add(): Argument #1 must be of type string, int given
<?xml version="1.0" encoding="UTF-8"?>
<root/>
