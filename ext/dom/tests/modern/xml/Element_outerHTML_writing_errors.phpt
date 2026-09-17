--TEST--
Test writing Element::$outerHTML on XML documents - error cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root/>');
try {
    $dom->documentElement->outerHTML = '<x/>';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$dom = Dom\XMLDocument::createFromString('<root><child/></root>');
try {
    $dom->documentElement->firstChild->outerHTML = '<!DOCTYPE html>';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Invalid Modification Error
DOMException: XML fragment is not well-formed
