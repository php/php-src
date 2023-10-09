--TEST--
DOMElement::prepend() with hierarchy changes and errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom_original = new DOMDocument;
$dom_original->loadXML('<p><b>hello</b><b><i>world</i></b></p>');

echo "-- Prepend hello with world --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
$b_world = $b_hello->nextSibling;
$b_hello->prepend($b_world);
var_dump($dom->saveHTML());

echo "-- Prepend hello with world's child --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
$b_world = $b_hello->nextSibling;
$b_hello->prepend($b_world->firstChild);
var_dump($dom->saveHTML());

echo "-- Prepend world's child with hello --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
$b_world = $b_hello->nextSibling;
$b_world->firstChild->prepend($b_hello);
var_dump($dom->saveHTML());

echo "-- Prepend hello with itself --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
try {
    $b_hello->prepend($b_hello);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->saveHTML());

echo "-- Prepend world's i tag with the parent --\n";
$dom = clone $dom_original;
$b_hello = $dom->firstChild->firstChild;
$b_world = $b_hello->nextSibling;
try {
    $b_world->firstChild->prepend($b_world);
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
-- Prepend hello with world --
string(39) "<p><b><b><i>world</i></b>hello</b></p>
"
-- Prepend hello with world's child --
string(39) "<p><b><i>world</i>hello</b><b></b></p>
"
-- Prepend world's child with hello --
string(39) "<p><b><i><b>hello</b>world</i></b></p>
"
-- Prepend hello with itself --
Hierarchy Request Error
string(39) "<p><b>hello</b><b><i>world</i></b></p>
"
-- Prepend world's i tag with the parent --
Hierarchy Request Error
string(39) "<p><b>hello</b><b><i>world</i></b></p>
"
-- Append from another document --
Wrong Document Error
string(13) "<p>other</p>
"
string(39) "<p><b>hello</b><b><i>world</i></b></p>
"
