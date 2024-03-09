--TEST--
ParentNode hierarchy exceptions with temporary and non-temporary text nodes
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();

try {
    $dom->append("bar");
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

try {
    $dom->append($dom->createTextNode("bar"));
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

$text = $dom->createTextNode("bar");
try {
    $dom->append($text);
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

var_dump($text->parentNode);
var_dump($text->textContent);

$element = $dom->createElement("container");
$text = $element->appendChild($dom->createTextNode("text"));
try {
    $dom->append($text);
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
Exception: Cannot insert text as a child of a document
Exception: Cannot insert text as a child of a document
Exception: Cannot insert text as a child of a document
NULL
string(3) "bar"
Exception: Cannot insert text as a child of a document
