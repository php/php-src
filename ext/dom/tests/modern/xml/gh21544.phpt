--TEST--
GH-21544 (Dom\XMLDocument::C14N() drops namespace declarations on DOM-built documents)
--CREDITS--
Toon Verwerft (veewee)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = Dom\XMLDocument::createEmpty();
$root = $doc->createElementNS("urn:envelope", "env:Root");
$doc->appendChild($root);
$child = $doc->createElementNS("urn:child", "x:Child");
$root->appendChild($child);

$parsed = Dom\XMLDocument::createFromString(
    '<env:Root xmlns:env="urn:envelope"><x:Child xmlns:x="urn:child"/></env:Root>'
);

echo "DOM-built C14N: " . $doc->C14N() . PHP_EOL;
echo "Parsed C14N:    " . $parsed->C14N() . PHP_EOL;
var_dump($doc->C14N() === $parsed->C14N());

?>
--EXPECT--
DOM-built C14N: <env:Root xmlns:env="urn:envelope"><x:Child xmlns:x="urn:child"></x:Child></env:Root>
Parsed C14N:    <env:Root xmlns:env="urn:envelope"><x:Child xmlns:x="urn:child"></x:Child></env:Root>
bool(true)
