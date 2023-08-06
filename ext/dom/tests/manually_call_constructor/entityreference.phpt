--TEST--
Manually call __construct() - entity reference variation
--EXTENSIONS--
dom
--FILE--
<?php

$entityRef = new DOMEntityReference('foo');
var_dump($entityRef->nodeName, $entityRef->textContent);
$entityRef->__construct('foo2');
var_dump($entityRef->nodeName, $entityRef->textContent);

$doc = new DOMDocument();
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container/>
XML);
$doc->documentElement->appendChild($entityRef);
echo $doc->saveXML();

$entityRef->__construct('foo3');
$doc->documentElement->appendChild($entityRef);
echo $doc->saveXML();

?>
--EXPECTF--
string(3) "foo"
string(0) ""

Fatal error: Uncaught Error: Couldn't fetch DOMEntityReference. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
