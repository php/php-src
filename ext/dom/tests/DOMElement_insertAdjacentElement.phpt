--TEST--
DOMElement::insertAdjacentElement()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML('<?xml version="1.0"?><container><p>foo</p></container>');
$container = $dom->documentElement;
$p = $container->firstElementChild;

echo "--- Edge cases ---\n";

var_dump($dom->createElement('free')->insertAdjacentElement("beforebegin", $dom->createElement('element')));
var_dump($dom->createElement('free')->insertAdjacentElement("afterend", $dom->createElement('element')));

try {
    var_dump($dom->createElement('free')->insertAdjacentElement("bogus", $dom->createElement('element')));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Hierarchy test ---\n";

$element = $dom->createElement('free');
$child = $element->appendChild($dom->createElement('child'));
foreach (['beforebegin', 'afterbegin', 'beforeend', 'afterend'] as $where) {
    try {
        var_dump($child->insertAdjacentElement($where, $element)->tagName);
    } catch (DOMException $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "--- Normal cases ---\n";

var_dump($p->insertAdjacentElement("beforebegin", $dom->createElement('A'))->tagName);
echo $dom->saveXML();

var_dump($p->insertAdjacentElement("afterbegin", $dom->createElement('B'))->tagName);
echo $dom->saveXML();

var_dump($p->insertAdjacentElement("beforeend", $dom->createElement('C'))->tagName);
echo $dom->saveXML();

var_dump($p->insertAdjacentElement("afterend", $dom->createElement('D'))->tagName);
echo $dom->saveXML();

$empty = $dom->createElement('empty');
var_dump($empty->insertAdjacentElement("afterbegin", $dom->createElement('A'))->tagName);
echo $dom->saveXML($empty), "\n";

echo "--- Namespace test ---\n";

$dom->loadXML('<?xml version="1.0"?><container xmlns:foo="some:ns"/>');
$dom->documentElement->insertAdjacentElement("afterbegin", $dom->createElementNS("some:ns", "bar"));
echo $dom->saveXML();

echo "--- Two document test ---\n";

$dom1 = new DOMDocument();
$dom1->loadXML('<?xml version="1.0"?><container><div/></container>');
$dom2 = new DOMDocument();
$dom2->loadXML('<?xml version="1.0"?><container><p/></container>');
$dom1->documentElement->firstChild->insertAdjacentElement('afterbegin', $dom2->documentElement->firstChild);
echo $dom1->saveXML();
echo $dom2->saveXML();

?>
--EXPECT--
--- Edge cases ---
NULL
NULL
Syntax Error
--- Hierarchy test ---
Hierarchy Request Error
Hierarchy Request Error
Hierarchy Request Error
Hierarchy Request Error
--- Normal cases ---
string(1) "A"
<?xml version="1.0"?>
<container><A/><p>foo</p></container>
string(1) "B"
<?xml version="1.0"?>
<container><A/><p><B/>foo</p></container>
string(1) "C"
<?xml version="1.0"?>
<container><A/><p><B/>foo<C/></p></container>
string(1) "D"
<?xml version="1.0"?>
<container><A/><p><B/>foo<C/></p><D/></container>
string(1) "A"
<empty><A/></empty>
--- Namespace test ---
<?xml version="1.0"?>
<container xmlns:foo="some:ns"><foo:bar/></container>
--- Two document test ---
<?xml version="1.0"?>
<container><div><p/></div></container>
<?xml version="1.0"?>
<container/>
