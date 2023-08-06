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
--EXPECTF--
string(3) "foo"
string(8) "my value"

Fatal error: Uncaught Error: Couldn't fetch DOMElement. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
