--TEST--
Manually call __construct() - element variation
--EXTENSIONS--
dom
--FILE--
<?php

$element = new DOMElement('foo', 'my value');
var_dump($element->nodeName, $element->textContent);
$element->__construct('foo2', 'my new value');
var_dump($element->nodeName, $element->textContent);

$doc = new DOMDocument();
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container/>
XML);
$doc->documentElement->appendChild($element);
echo $doc->saveXML();

$element->__construct('foo3', 'my new new value');
$doc->documentElement->appendChild($element);
echo $doc->saveXML();

?>
--EXPECT--
string(3) "foo"
string(8) "my value"
string(4) "foo2"
string(12) "my new value"
<?xml version="1.0"?>
<container><foo2>my new value</foo2></container>
<?xml version="1.0"?>
<container><foo2>my new value</foo2><foo3>my new new value</foo3></container>
