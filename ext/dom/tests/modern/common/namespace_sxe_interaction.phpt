--TEST--
Serialization interaction between simplexml and dom for namespaces
--EXTENSIONS--
dom
simplexml
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root/>');
$sxe = simplexml_import_dom($dom);

$sxe->addAttribute('a:attr', 'value', 'urn:a');
$sxe->addChild('b:child', 'value', 'urn:b');
$sxe->addChild('foo', 'value2');
$dom->documentElement->firstElementChild->appendChild($dom->createElementNS('urn:c', 'c:child'));

echo "namespace c: ";
var_dump($dom->documentElement->firstElementChild->firstElementChild->lookupNamespaceURI('c'));
echo "namespace b: ";
var_dump($dom->documentElement->firstElementChild->firstElementChild->lookupNamespaceURI('b'));
echo "namespace a: ";
var_dump($dom->documentElement->firstElementChild->firstElementChild->lookupNamespaceURI('a'));

echo "=== serialize SimpleXML ===\n";

echo $sxe->saveXML(), "\n";
echo $sxe->foo->saveXML(), "\n";
$sxe->asXML(__DIR__ . "/namespace_sxe_interaction1.xml");
$sxe->foo->asXML(__DIR__ . "/namespace_sxe_interaction2.xml");
echo file_get_contents(__DIR__ . "/namespace_sxe_interaction1.xml"), "\n";
echo file_get_contents(__DIR__ . "/namespace_sxe_interaction2.xml"), "\n";

echo "=== serialize DOM ===\n";

echo $dom->saveXML(), "\n\n";

echo "=== serialize imported DOM ===\n";

// Importing should yield the exact same document
$new_dom = Dom\XMLDocument::createEmpty();
$new_dom->append($new_dom->importNode($dom->documentElement, true));
echo $new_dom->saveXML(), "\n";

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/namespace_sxe_interaction1.xml");
@unlink(__DIR__ . "/namespace_sxe_interaction2.xml");
?>
--EXPECT--
namespace c: string(5) "urn:c"
namespace b: string(5) "urn:b"
namespace a: NULL
=== serialize SimpleXML ===
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:a="urn:a" a:attr="value"><b:child xmlns:b="urn:b">value<c:child xmlns:c="urn:c"/></b:child><foo>value2</foo></root>
<foo>value2</foo>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:a="urn:a" a:attr="value"><b:child xmlns:b="urn:b">value<c:child xmlns:c="urn:c"/></b:child><foo>value2</foo></root>
<foo>value2</foo>
=== serialize DOM ===
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:a="urn:a" a:attr="value"><b:child xmlns:b="urn:b">value<c:child xmlns:c="urn:c"/></b:child><foo>value2</foo></root>

=== serialize imported DOM ===
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:a="urn:a" a:attr="value"><b:child xmlns:b="urn:b">value<c:child xmlns:c="urn:c"/></b:child><foo>value2</foo></root>
