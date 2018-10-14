--TEST--
XMLReader: Schema validation
--SKIPIF--
<?php if (!extension_loaded('xmlreader')) die('skip');?>
<?php if (!method_exists('XMLReader','setSchema')) die('skip XMLReader::setSchema() not supported');?>
--FILE--
<?php
/* $Id$ */

$xml =<<<EOF
<?xml version="1.0" encoding="UTF-8" ?>
<items>
  <item>123</item>
  <item>456</item>
</items>
EOF;

$reader = new XMLReader();
$reader->XML($xml);
$reader->setSchema(dirname(__FILE__) . '/013.xsd');
while($reader->read()) {
	if ($reader->nodeType == XMLReader::ELEMENT && $reader->name == 'item') {
		$reader->read();
		var_dump($reader->value);
	}
}
$reader->close();

?>
===FAIL===
<?php

$xml =<<<EOF
<?xml version="1.0" encoding="UTF-8" ?>
<foo/>
EOF;

$reader = new XMLReader();
$reader->XML($xml);
$reader->setSchema(dirname(__FILE__) . '/013.xsd');
while($reader->read() && $reader->nodeType != XMLReader::ELEMENT);
$reader->close();

?>
===DONE===
--EXPECTF--
string(3) "123"
string(3) "456"
===FAIL===

Warning: XMLReader::read(): Element 'foo': %s
===DONE===
