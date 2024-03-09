--TEST--
DOM\Element::setAttributeNode(NS) adopting from another document
--EXTENSIONS--
dom
--FILE--
<?php

$dom1 = DOM\HTMLDocument::createEmpty();
$container = $dom1->appendChild($dom1->createElement("container"));

echo "--- Without namespace ---\n";

$dom2 = DOM\HTMLDocument::createEmpty();
$attr = $dom2->createAttribute("my-attribute");
$attr->value = "1";

$container->setAttributeNode($attr);
var_dump($attr->ownerDocument === $dom1);

// Should not cause problems
unset($dom2);
unset($attr);

echo "--- With namespace ---\n";

$dom2 = DOM\HTMLDocument::createEmpty();
$attr2 = $dom2->createAttributeNS("urn:a", "a:my-attribute");
$attr2->value = "2";
$dom1->documentElement->setAttributeNode($attr2);

$dom2 = DOM\HTMLDocument::createEmpty();
$attr3 = $dom2->createAttributeNS("urn:b", "a:my-attribute");
$attr3->value = "3";
$dom1->documentElement->setAttributeNode($attr3);

var_dump($attr2->prefix, $attr2->namespaceURI, $attr2->localName);
var_dump($attr3->prefix, $attr3->namespaceURI, $attr3->localName);

echo "--- Resulting document ---\n";

echo $dom1->saveHTML(), "\n";

?>
--EXPECT--
--- Without namespace ---
bool(true)
--- With namespace ---
string(1) "a"
string(5) "urn:a"
string(12) "my-attribute"
string(1) "a"
string(5) "urn:b"
string(12) "my-attribute"
--- Resulting document ---
<container my-attribute="1" a:my-attribute="2" a:my-attribute="3"></container>
