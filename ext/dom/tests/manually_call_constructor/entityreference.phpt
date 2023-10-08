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
--EXPECT--
string(3) "foo"
string(0) ""
string(4) "foo2"
string(0) ""
<?xml version="1.0"?>
<container>&foo2;</container>
<?xml version="1.0"?>
<container>&foo2;&foo3;</container>
