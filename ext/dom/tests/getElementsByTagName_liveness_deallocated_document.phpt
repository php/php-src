--TEST--
getElementsByTagName() liveness with deallocated document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <p>1</p><p>2</p><p>3</p>
</container>
XML);

$ps = $dom->documentElement->getElementsByTagName('p');
$second = $ps->item(1);
var_dump($second->textContent);
var_dump($ps->length);

unset($dom);
$dom = $second->ownerDocument;

$second->parentNode->appendChild($dom->createElement('p', '4'));
var_dump($ps->length);

?>
--EXPECT--
string(1) "2"
int(3)
int(3)
