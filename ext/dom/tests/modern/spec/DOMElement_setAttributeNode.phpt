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
$container->setAttributeNode($attribute1);
$attribute2 = $dom1->createAttribute("my-attribute");
$attribute2->value = "2";
var_dump($container->setAttributeNode($attribute2) === $attribute1);

echo "--- With namespace ---\n";

$attribute3 = $dom1->createAttributeNS("urn:a", "my-attribute");
$attribute3->value = "3";
$container->setAttributeNode($attribute3);
$container->setAttributeNode($attribute3);
$attribute4 = $dom1->createAttributeNS("urn:b", "my-attribute");
$attribute4->value = "4";
var_dump($container->setAttributeNodeNS($attribute4) === $attribute3);

echo "--- Resulting document ---\n";

echo $dom1->saveHTML(), "\n";

?>
--EXPECT--
--- Without namespace ---
bool(true)
--- With namespace ---
bool(false)
--- Resulting document ---
<container my-attribute="2" my-attribute="3" my-attribute="4"></container>
