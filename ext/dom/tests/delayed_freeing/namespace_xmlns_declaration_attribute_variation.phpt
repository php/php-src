--TEST--
Delayed freeing namespace xmlns declaration - attribute variation
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML('<?xml version="1.0"?><container xmlns="http://php.net"/>');
$doc->documentElement->appendChild($doc->createElementNS('http://php.net', 'example'));
echo $doc->saveXML(), "\n";

$declaration = $doc->documentElement->getAttributeNode('xmlns');
var_dump($declaration->nodeValue);

$doc->documentElement->remove();
echo $doc->saveXML(), "\n";

var_dump($declaration->nodeValue);
unset($doc);
var_dump($declaration->parentNode->nodeName);
var_dump($declaration->parentNode->childNodes[0]->nodeName);

$declaration->parentNode->childNodes[0]->remove();
var_dump($declaration->parentNode->nodeName);
var_dump($declaration->parentNode->childNodes[0]);
?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns="http://php.net"><example/></container>

string(14) "http://php.net"
<?xml version="1.0"?>

string(14) "http://php.net"
string(9) "container"
string(7) "example"
string(9) "container"
NULL
