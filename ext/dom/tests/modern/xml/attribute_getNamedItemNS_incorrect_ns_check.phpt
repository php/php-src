--TEST--
getNamedItemNS() incorrect namespace check
--EXTENSIONS--
dom
--CREDITS--
veewee
--FILE--
<?php 

$xml = Dom\XMLDocument::createFromString(<<<EOXML
<?xml version="1.0" encoding="UTF-8"?>
<note
      xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
      xsi:noNamespaceSchemaLocation="note-nonamespace1.xsd http://localhost/note-nonamespace2.xsd"
      xsi:schemaLocation="http://www.happy-helpers1.com note-namespace1.xsd http://www.happy-helpers2.com http://localhost/note-namespace2.xsd">
</note>
EOXML
);
$documentElement = $xml->documentElement;
$attributes = $documentElement->attributes;
$schemaLocation = $attributes->getNamedItemNS('http://www.w3.org/2001/XMLSchema-instance', 'schemaLocation');
var_dump($schemaLocation->textContent);

?>
--EXPECT--
string(116) "http://www.happy-helpers1.com note-namespace1.xsd http://www.happy-helpers2.com http://localhost/note-namespace2.xsd"
