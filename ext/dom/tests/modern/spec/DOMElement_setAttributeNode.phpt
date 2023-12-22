--TEST--
DOMElement::setAttributeNode(NS) in the same document
--EXTENSIONS--
dom
--FILE--
<?php

$dom1 = DOM\HTMLDocument::createEmpty();
$container = $dom1->appendChild($dom1->createElement("container"));

echo "--- Without namespace ---\n";

$attribute1 = $dom1->createAttribute("my-attribute");
$attribute1->value = "1";
$container->setAttributeNode($attribute1);
var_dump($container->setAttributeNode($attribute1) === null);
$attribute2 = $dom1->createAttribute("my-attribute");
$attribute2->value = "2";
var_dump($container->setAttributeNode($attribute2) === $attribute1);
$attribute3 = $dom1->createAttributeNS("", "my-ATTRIBUTE");
$attribute3->value = "3";
var_dump($container->setAttributeNode($attribute3) === null);

echo "--- With namespace ---\n";

$attribute4 = $dom1->createAttributeNS("urn:a", "my-attribute");
$attribute4->value = "4";
$container->setAttributeNode($attribute4);
var_dump($container->setAttributeNode($attribute4) === null);
$attribute5 = $dom1->createAttributeNS("urn:b", "my-attribute");
$attribute5->value = "5";
var_dump($container->setAttributeNodeNS($attribute5) === null);

echo "--- Resulting document ---\n";

echo $dom1->saveHTML(), "\n";

?>
--EXPECT--
--- Without namespace ---
bool(true)
bool(true)
bool(true)
--- With namespace ---
bool(true)
bool(true)
--- Resulting document ---
<container my-attribute="2" my-ATTRIBUTE="3" my-attribute="4" my-attribute="5"></container>
