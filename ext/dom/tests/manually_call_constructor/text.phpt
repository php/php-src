--TEST--
Manually call __construct() - text variation
--EXTENSIONS--
dom
--FILE--
<?php

$text = new DOMText('my value');
var_dump($text->textContent);
$text->__construct('my new value');
var_dump($text->textContent);

$doc = new DOMDocument();
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container/>
XML);
$doc->documentElement->appendChild($text);
echo $doc->saveXML();

$text->__construct("\nmy new new value");
$doc->documentElement->appendChild($text);
echo $doc->saveXML();

?>
--EXPECTF--
string(8) "my value"

Fatal error: Uncaught Error: Couldn't fetch DOMText. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
