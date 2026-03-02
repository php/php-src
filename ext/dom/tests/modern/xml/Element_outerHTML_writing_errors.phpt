--TEST--
Test writing Element::$outerHTML on XML documents - error cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root/>');
try {
    $dom->documentElement->outerHTML = '<x/>';
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

$dom = Dom\XMLDocument::createFromString('<root><child/></root>');
try {
    $dom->documentElement->firstChild->outerHTML = '<!DOCTYPE html>';
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Invalid Modification Error
XML fragment is not well-formed
