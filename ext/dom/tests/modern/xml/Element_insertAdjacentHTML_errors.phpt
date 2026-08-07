--TEST--
Dom\Element::insertAdjacentHTML() with XML nodes - errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root/>');
try {
    $dom->documentElement->insertAdjacentHTML(Dom\AdjacentPosition::AfterBegin, "<non-well-formed>");
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: XML fragment is not well-formed
