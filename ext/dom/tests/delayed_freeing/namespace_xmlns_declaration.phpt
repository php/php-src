--TEST--
Delayed freeing namespace xmlns declaration
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML('<?xml version="1.0"?><container xmlns="http://php.net"/>');
$doc->documentElement->appendChild($el = $doc->createElementNS('http://php.net', 'example'));
echo $doc->saveXML(), "\n";

$doc->documentElement->remove();
echo $doc->saveXML(), "\n";

var_dump($el->namespaceURI);
?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns="http://php.net"><example/></container>

<?xml version="1.0"?>

string(14) "http://php.net"
