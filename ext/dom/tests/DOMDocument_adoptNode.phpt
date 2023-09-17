--TEST--
Tests DOMDocument::adoptNode()
--EXTENSIONS--
dom
--FILE--
<?php

$doc1 = new DOMDocument();
$doc1->loadXML("<p><b>hi<i attrib=\"1\">x</i></b>world</p>");
$doc2 = new DOMDocument();
$doc2->loadXML("<div/>");

$b_tag_element = $doc1->firstChild->firstChild;
$i_tag_element = $b_tag_element->lastChild;

echo "-- Owner document check before adopting --\n";
var_dump($i_tag_element->ownerDocument === $doc1);
var_dump($i_tag_element->ownerDocument === $doc2);

echo "-- Trying to append child from other document --\n";
try {
    $doc2->firstChild->appendChild($b_tag_element); // Should fail because it's another document
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "-- Adopting --\n";
$adopted = $doc2->adoptNode($b_tag_element);
var_dump($adopted->textContent);
var_dump($doc1->saveXML());
var_dump($doc2->saveXML());

echo "-- Appending the adopted node --\n";

$doc2->firstChild->appendChild($adopted);
var_dump($doc2->saveXML());
var_dump($i_tag_element->ownerDocument === $doc1);
var_dump($i_tag_element->ownerDocument === $doc2);

echo "-- Adopt node to the original document --\n";

$adopted = $doc1->adoptNode($doc1->firstChild->firstChild);
var_dump($adopted->textContent);
var_dump($doc1->saveXML());

echo "-- Adopt a document (strict error on) --\n";

try {
    $doc1->adoptNode($doc1);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "-- Adopt a document (strict error off) --\n";

$doc1->strictErrorChecking = false;
try {
    $doc1->adoptNode($doc1);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}
$doc1->strictErrorChecking = true;

echo "-- Adopt an attribute --\n";

$doc3 = new DOMDocument();
$doc3->loadXML('<p align="center">hi</p>');
$attribute = $doc3->firstChild->attributes->item(0);
var_dump($attribute->parentNode !== NULL);
$adopted = $doc3->adoptNode($attribute);
var_dump($adopted->parentNode === NULL);
echo $doc3->saveXML();

echo "-- Append an attribute from another document --\n";

$doc4 = new DOMDocument();
$doc4->appendChild($doc4->createElement('container'));
$doc4->documentElement->appendChild($doc4->adoptNode($adopted));
echo $doc4->saveXML();

echo "-- Adopt an entity reference --\n";

$doc4 = new DOMDocument();
$doc4->loadXML(<<<'XML'
<?xml version='1.0' encoding='utf-8' ?>
<!DOCTYPE set PUBLIC "-//OASIS//DTD DocBook XML V5.0//EN" "http://www.docbook.org/xml/5.0/dtd/docbook.dtd" [
<!ENTITY my_entity '<p>hi</p>'> ]>
<p/>
XML, LIBXML_NOENT);
$p_tag_element = $doc4->firstChild->nextSibling;
$entity_reference = $doc4->createEntityReference('my_entity');
$p_tag_element->appendChild($entity_reference);
var_dump($doc4->saveXML());
$doc3->adoptNode($entity_reference);
var_dump($doc4->saveXML());
$doc3->firstChild->appendChild($entity_reference);
var_dump($doc3->saveXML());

echo "-- Adopt a node and destroy the new document --\n";

$doc1 = new DOMDocument();
$doc1->appendChild($doc1->createElement('child'));
$doc2 = new DOMDocument();
$doc2->appendChild($doc2->createElement('container'));
$doc2->documentElement->appendChild($child = $doc2->adoptNode($doc1->documentElement));
echo $doc1->saveXML();
echo $doc2->saveXML();
// Try to trigger a use-after-free
unset($doc2);
var_dump($child->nodeName);
unset($doc1);
var_dump($child->nodeName);

?>
--EXPECTF--
-- Owner document check before adopting --
bool(true)
bool(false)
-- Trying to append child from other document --
Wrong Document Error
-- Adopting --
string(3) "hix"
string(35) "<?xml version="1.0"?>
<p>world</p>
"
string(29) "<?xml version="1.0"?>
<div/>
"
-- Appending the adopted node --
string(62) "<?xml version="1.0"?>
<div><b>hi<i attrib="1">x</i></b></div>
"
bool(false)
bool(true)
-- Adopt node to the original document --
string(5) "world"
string(27) "<?xml version="1.0"?>
<p/>
"
-- Adopt a document (strict error on) --
Not Supported Error
-- Adopt a document (strict error off) --

Warning: DOM\Document::adoptNode(): Not Supported Error in %s on line %d
-- Adopt an attribute --
bool(true)
bool(true)
<?xml version="1.0"?>
<p>hi</p>
-- Append an attribute from another document --
<?xml version="1.0"?>
<container align="center"/>
-- Adopt an entity reference --
string(202) "<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE set PUBLIC "-//OASIS//DTD DocBook XML V5.0//EN" "http://www.docbook.org/xml/5.0/dtd/docbook.dtd" [
<!ENTITY my_entity "<p>hi</p>">
]>
<p>&my_entity;</p>
"
string(188) "<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE set PUBLIC "-//OASIS//DTD DocBook XML V5.0//EN" "http://www.docbook.org/xml/5.0/dtd/docbook.dtd" [
<!ENTITY my_entity "<p>hi</p>">
]>
<p/>
"
string(43) "<?xml version="1.0"?>
<p>hi&my_entity;</p>
"
-- Adopt a node and destroy the new document --
<?xml version="1.0"?>
<?xml version="1.0"?>
<container><child/></container>
string(5) "child"
string(5) "child"
