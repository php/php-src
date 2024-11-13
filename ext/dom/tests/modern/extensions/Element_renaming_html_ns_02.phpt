--TEST--
Element renaming interaction with the HTML namespace 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();
$el = $dom->createElementNS("http://www.w3.org/1999/xhtml", "foo:bar");
try {
    $el->rename("urn:a", "foo:baz");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
$el = $dom->createElementNS("urn:a", "foo:bar");
try {
    $el->rename("http://www.w3.org/1999/xhtml", "foo:baz");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
It is not possible to move an element out of the HTML namespace because the HTML namespace is tied to the HTMLElement class
It is not possible to move an element into the HTML namespace because the HTML namespace is tied to the HTMLElement class
