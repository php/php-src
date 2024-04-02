--TEST--
ParentNode/ChildNode pre-insertion validation
--EXTENSIONS--
dom
--FILE--
<?php
$dom = DOM\XMLDocument::createFromString("<!DOCTYPE root><root/>");
$doctype = $dom->doctype;
$dom->removeChild($doctype);

echo "--- Trying to insert text node into the document ---\n";

try {
    $dom->append("foo", "bar", "baz");
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    $dom->append($dom->createTextNode("text node"));
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    $dom->append($dom->createCDATASection("text node"));
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "--- Trying to insert doctype into not a document ---\n";

$element = $dom->createElement("foo");
try {
    $element->append($doctype);
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "--- Trying to insert doctype at the wrong place in a document ---\n";

try {
    $dom->append($doctype);
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "--- Prepend doctype in a document should work ---\n";

$dom->prepend($doctype);

echo "--- Trying to create multiple document roots ---\n";

try {
    $dom->append($dom->createElement("foo"));
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "--- Trying to insert an element before a document type ---\n";

$dom->documentElement->remove();
try {
    $dom->prepend($element);
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "--- Document output ---\n";

echo $dom->saveXML(), "\n";

echo "--- Document fragment edge cases with multiple elements ---\n";

$dom = DOM\XMLDocument::createEmpty();
$fragment = $dom->createDocumentFragment();
$fragment->append($dom->createElement("foo"));
$fragment->append($dom->createElement("bar"));
try {
    $dom->append($fragment);
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "--- Document fragment edge cases with text ---\n";

$dom = DOM\XMLDocument::createEmpty();
$fragment = $dom->createDocumentFragment();
$fragment->append("foo");
$fragment->append($dom->createCDATASection("bar"));
try {
    $dom->append($fragment);
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
--- Trying to insert text node into the document ---
Exception: Cannot insert text as a child of a document
Exception: Cannot insert text as a child of a document
Exception: Cannot insert text as a child of a document
--- Trying to insert doctype into not a document ---
Exception: Cannot insert a document type into anything other than a document
--- Trying to insert doctype at the wrong place in a document ---
Exception: Document types must be the first child in a document
--- Prepend doctype in a document should work ---
--- Trying to create multiple document roots ---
Exception: Cannot have more than one element child in a document
--- Trying to insert an element before a document type ---
Exception: Document types must be the first child in a document
--- Document output ---
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root>

--- Document fragment edge cases with multiple elements ---
Exception: Cannot have more than one element child in a document
--- Document fragment edge cases with text ---
Exception: Cannot insert text as a child of a document
