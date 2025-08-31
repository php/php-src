--TEST--
DOMParentNode::replaceChildren()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadHTML('<!DOCTYPE HTML><html><p>hi</p> test <p>hi2</p></html>');

echo "--- Edge cases ---\n";

try {
    $dom->documentElement->replaceChildren($dom->documentElement);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $dom->documentElement->firstElementChild->replaceChildren($dom->documentElement);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Normal cases ---\n";

$dom->documentElement->replaceChildren('foo', $dom->createElement('p'), 'bar');
echo $dom->saveXML();

$fragment1 = $dom->createDocumentFragment();
$fragment1->appendChild($dom->createElement('a'));
$fragment1->appendChild($dom->createElement('b'));
$fragment2 = $dom->createDocumentFragment();
$fragment2->append('text');
$fragment3 = $dom->createDocumentFragment();
$dom->documentElement->replaceChildren($fragment1, $fragment2, $fragment3);
echo $dom->saveXML();

echo "--- Fragment case ---\n";

$fragment = $dom->createDocumentFragment();
$fragment->replaceChildren();
var_dump($dom->saveXML($fragment));

$old = $fragment->appendChild($dom->createElement('p', 'test'));
$fragment->replaceChildren($dom->createElement('b', 'test'));
echo $dom->saveXML($fragment), "\n";
var_dump($old->nodeValue);

echo "--- Idempotent case ---\n";

$dom->replaceChildren($dom->documentElement);
echo $dom->saveXML();

echo "--- Removal shortcut ---\n";

$dom->documentElement->replaceChildren();
echo $dom->saveXML();

?>
--EXPECT--
--- Edge cases ---
Hierarchy Request Error
Hierarchy Request Error
--- Normal cases ---
<?xml version="1.0" standalone="yes"?>
<!DOCTYPE HTML>
<html>foo<p/>bar</html>
<?xml version="1.0" standalone="yes"?>
<!DOCTYPE HTML>
<html><a/><b/>text</html>
--- Fragment case ---
string(0) ""
<b>test</b>
string(4) "test"
--- Idempotent case ---
<?xml version="1.0" standalone="yes"?>
<html><a/><b/>text</html>
--- Removal shortcut ---
<?xml version="1.0" standalone="yes"?>
<html/>
