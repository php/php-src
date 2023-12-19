--TEST--
Consistency of parent node after exception in ParentNode::append()
--EXTENSIONS--
dom
--FILE--
<?php
$dom = DOM\XMLDocument::createEmpty();
$fragment = $dom->createDocumentFragment();
$fragment->append($element = $dom->createElement("foo"));
$fragment->append($dom->createElement("bar"));
try {
    $dom->append($fragment);
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

var_dump($element->parentNode);
?>
--EXPECT--
Exception: Cannot have more than one element child in a document
NULL
