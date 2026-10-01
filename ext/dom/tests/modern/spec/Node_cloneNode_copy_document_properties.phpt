--TEST--
Dom\Node::cloneNode() should copy the document properties
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$dom = $dom->cloneNode();

$dom->appendChild($dom->createElement("foo"));

try {
    $dom->prepend($dom->createElement("bar"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Cannot have more than one element child in a document
