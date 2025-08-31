--TEST--
Node renaming validation errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root/>');

try {
    $dom->documentElement->rename('', 'a:b');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $dom->documentElement->rename('urn:a', 'a:b:c');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo $dom->saveXML();

?>
--EXPECT--
Namespace Error
Invalid Character Error
<?xml version="1.0" encoding="UTF-8"?>
<root/>
