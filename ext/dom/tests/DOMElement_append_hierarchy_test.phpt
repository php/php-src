--TEST--
DOMElement::append() with hierarchy changes and errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom_original = new DOMDocument;
$dom_original->loadXML('<p><b>hello</b><b><i>world</i></b></p>');

echo "-- Append hello with world --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
$b_world = $b_hello->nextSibling;
$b_hello->append($b_world);
var_dump($dom->saveHTML());

echo "-- Append hello with world's child --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
$b_world = $b_hello->nextSibling;
$b_hello->append($b_world->firstChild);
var_dump($dom->saveHTML());

echo "-- Append hello with world's child and text --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
$b_world = $b_hello->nextSibling;
$b_hello->append($b_world->firstChild, "foo");
var_dump($dom->saveHTML());

echo "-- Append world's child with hello --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
$b_world = $b_hello->nextSibling;
$b_world->firstChild->append($b_hello);
var_dump($dom->saveHTML());

echo "-- Append hello with itself --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
try {
    $b_hello->append($b_hello);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->saveHTML());

echo "-- Append hello with itself and text --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
try {
    $b_hello->append($b_hello, "foo");
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->saveHTML());

echo "-- Append world's i tag with the parent --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
$b_world = $b_hello->nextSibling;
try {
    $b_world->firstChild->append($b_world);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->saveHTML());

echo "-- Append from another document --\n";
$dom = clone $dom_original;
$dom2 = new DOMDocument;
$dom2->loadXML('<p>other</p>');
try {
    $dom->firstChild->firstChild->prepend($dom2->firstChild);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom2->saveHTML());
var_dump($dom->saveHTML());

?>
--EXPECT--
-- Append hello with world --
string(39) "<p><b>hello<b><i>world</i></b></b></p>
"
-- Append hello with world's child --
string(39) "<p><b>hello<i>world</i></b><b></b></p>
"
-- Append hello with world's child and text --
string(42) "<p><b>hello<i>world</i>foo</b><b></b></p>
"
-- Append world's child with hello --
string(39) "<p><b><i>world<b>hello</b></i></b></p>
"
-- Append hello with itself --
Hierarchy Request Error
string(39) "<p><b>hello</b><b><i>world</i></b></p>
"
-- Append hello with itself and text --
Hierarchy Request Error
string(27) "<p><b><i>world</i></b></p>
"
-- Append world's i tag with the parent --
Hierarchy Request Error
string(39) "<p><b>hello</b><b><i>world</i></b></p>
"
-- Append from another document --
Wrong Document Error
string(13) "<p>other</p>
"
string(39) "<p><b>hello</b><b><i>world</i></b></p>
"
