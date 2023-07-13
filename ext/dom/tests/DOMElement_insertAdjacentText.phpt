--TEST--
DOMElement::insertAdjacentText()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML('<?xml version="1.0"?><container><p>foo</p></container>');
$container = $dom->documentElement;
$p = $container->firstElementChild;

echo "--- Edge cases ---\n";

try {
    $dom->createElement('free')->insertAdjacentText("bogus", "bogus");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Normal cases ---\n";

$p->insertAdjacentText("beforebegin", 'A');
echo $dom->saveXML();

$p->insertAdjacentText("afterbegin", 'B');
echo $dom->saveXML();

$p->insertAdjacentText("beforeend", 'C');
echo $dom->saveXML();

$p->insertAdjacentText("afterend", 'D');
echo $dom->saveXML();

$empty = $dom->createElement('empty');
$empty->insertAdjacentText("afterbegin", 'A');
echo $dom->saveXML($empty), "\n";

$AText = $empty->firstChild;
$empty->insertAdjacentText("afterbegin", 'B');
echo $dom->saveXML($empty), "\n";
var_dump($AText->textContent);

?>
--EXPECT--
--- Edge cases ---
Syntax Error
--- Normal cases ---
<?xml version="1.0"?>
<container>A<p>foo</p></container>
<?xml version="1.0"?>
<container>A<p>Bfoo</p></container>
<?xml version="1.0"?>
<container>A<p>BfooC</p></container>
<?xml version="1.0"?>
<container>A<p>BfooC</p>D</container>
<empty>A</empty>
<empty>BA</empty>
string(2) "BA"
