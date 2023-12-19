--TEST--
DOMElement::insertAdjacentText()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML('<?xml version="1.0"?><container><p>foo</p></container>');

echo "--- Edge cases ---\n";

try {
    $dom->createElement('free')->insertAdjacentText("bogus", "bogus");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

function testNormalCases($dom, $uppercase) {
    $container = $dom->documentElement;
    $p = $container->firstElementChild;
    $transform = fn ($s) => $uppercase ? strtoupper($s) : $s;

    $p->insertAdjacentText("beforebegin", 'A');
    echo $dom->saveXML();

    $p->insertAdjacentText("afterbegin", 'B');
    echo $dom->saveXML();

    $p->insertAdjacentText("beforeend", 'C');
    echo $dom->saveXML();

    $p->insertAdjacentText("afterend", 'D');
    echo $dom->saveXML();
}

echo "--- Normal cases uppercase ---\n";

testNormalCases(clone $dom, true);

echo "--- Normal cases lowercase ---\n";

testNormalCases($dom, false);

echo "--- Normal cases starting from empty element ---\n";

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
--- Normal cases uppercase ---
<?xml version="1.0"?>
<container>A<p>foo</p></container>
<?xml version="1.0"?>
<container>A<p>Bfoo</p></container>
<?xml version="1.0"?>
<container>A<p>BfooC</p></container>
<?xml version="1.0"?>
<container>A<p>BfooC</p>D</container>
--- Normal cases lowercase ---
<?xml version="1.0"?>
<container>A<p>foo</p></container>
<?xml version="1.0"?>
<container>A<p>Bfoo</p></container>
<?xml version="1.0"?>
<container>A<p>BfooC</p></container>
<?xml version="1.0"?>
<container>A<p>BfooC</p>D</container>
--- Normal cases starting from empty element ---
<empty>A</empty>
<empty>BA</empty>
string(1) "A"
