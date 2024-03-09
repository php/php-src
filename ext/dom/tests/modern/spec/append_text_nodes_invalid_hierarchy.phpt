--TEST--
Document::append() with text nodes in an invalid hierarchy
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();

try {
    $dom->append("foo", "bar", "baz", $dom);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $dom->append("foo", "bar", "baz");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

var_dump($dom->saveHTML());

?>
--EXPECT--
Hierarchy Request Error
Cannot insert text as a child of a document
string(0) ""
