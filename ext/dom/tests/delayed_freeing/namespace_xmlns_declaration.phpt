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
--EXPECTF--
<?xml version="1.0"?>
<container xmlns="http://php.net"><example/></container>

<?xml version="1.0"?>


Fatal error: Uncaught Error: Couldn't fetch DOMElement. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
