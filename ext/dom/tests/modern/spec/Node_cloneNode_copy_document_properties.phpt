--TEST--
DOM\Node::cloneNode() should copy the document properties
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$dom = $dom->cloneNode();

$dom->appendChild($dom->createElement("foo"));

try {
    $dom->prepend($dom->createElement("bar"));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot have more than one element child in a document
