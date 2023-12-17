--TEST--
appendChild() with DocumentType
--EXTENSIONS--
dom
--FILE--
<?php

$original = new DOMDocument();
$original->loadXML(<<<XML
<?xml version="1.0"?>
<!DOCTYPE doc [
    <!ENTITY foo "bar">
]>
<doc/>
XML);
$doctype = $original->doctype->cloneNode();
foreach ($doctype->entities as $entity) {
    echo "Found entity: ", $entity->nodeName, "\n";
}

$other = new DOMDocument();
$doctype = $other->importNode($original->doctype);
$other->appendChild($doctype);
$other->appendChild($doctype);
try {
    $other->appendChild($other->implementation->createDocumentType('doc', '', ''));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $other->saveXML();

?>
--EXPECT--
Found entity: foo
Document types must be the first child in a document
<?xml version="1.0"?>
<!DOCTYPE doc [
<!ENTITY foo "bar">
]>
