--TEST--
ParentNode/ChildNode pre-insertion validation
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\XMLDocument::createFromString("<!DOCTYPE root><root/>");
$doctype = $dom->doctype;
$dom->removeChild($doctype);

echo "--- Trying to insert text node into the document ---\n";

try {
    $dom->append("foo", "bar", "baz");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->append($dom->createTextNode("text node"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->append($dom->createCDATASection("text node"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "--- Trying to insert doctype into not a document ---\n";

$element = $dom->createElement("foo");
try {
    $element->append($doctype);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "--- Trying to insert doctype at the wrong place in a document ---\n";

try {
    $dom->append($doctype);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "--- Prepend doctype in a document should work ---\n";

$dom->prepend($doctype);

echo "--- Trying to create multiple document roots ---\n";

try {
    $dom->append($dom->createElement("foo"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "--- Trying to insert an element before a document type ---\n";

$dom->documentElement->remove();
try {
    $dom->prepend($element);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "--- Document output ---\n";

echo $dom->saveXml(), "\n";

echo "--- Document fragment edge cases with multiple elements ---\n";

$dom = Dom\XMLDocument::createEmpty();
$fragment = $dom->createDocumentFragment();
$fragment->append($dom->createElement("foo"));
$fragment->append($dom->createElement("bar"));
try {
    $dom->append($fragment);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "--- Document fragment edge cases with text ---\n";

$dom = Dom\XMLDocument::createEmpty();
$fragment = $dom->createDocumentFragment();
$fragment->append("foo");
$fragment->append($dom->createCDATASection("bar"));
try {
    $dom->append($fragment);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
--- Trying to insert text node into the document ---
DOMException: Cannot insert text as a child of a document
DOMException: Cannot insert text as a child of a document
DOMException: Cannot insert text as a child of a document
--- Trying to insert doctype into not a document ---
DOMException: Cannot insert a document type into anything other than a document
--- Trying to insert doctype at the wrong place in a document ---
DOMException: Document types must be the first child in a document
--- Prepend doctype in a document should work ---
--- Trying to create multiple document roots ---
DOMException: Cannot have more than one element child in a document
--- Trying to insert an element before a document type ---
DOMException: Document types must be the first child in a document
--- Document output ---
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root>

--- Document fragment edge cases with multiple elements ---
DOMException: Cannot have more than one element child in a document
--- Document fragment edge cases with text ---
DOMException: Cannot insert text as a child of a document
