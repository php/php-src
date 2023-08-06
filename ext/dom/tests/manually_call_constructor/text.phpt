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
--EXPECT--
string(8) "my value"
string(12) "my new value"
<?xml version="1.0"?>
<container>my new value</container>
<?xml version="1.0"?>
<container>my new value
my new new value</container>
