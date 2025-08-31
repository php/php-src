--TEST--
XMLReader: Schema validation
--EXTENSIONS--
xmlreader
--SKIPIF--
<?php if (!method_exists('XMLReader','setSchema')) die('skip XMLReader::setSchema() not supported');?>
--FILE--
<?php

$xml =<<<EOF
<?xml version="1.0" encoding="UTF-8" ?>
<items>
  <item>123</item>
  <item>456</item>
</items>
EOF;

$reader = new XMLReader();
$reader->XML($xml);
$reader->setSchema(__DIR__ . '/013.xsd');
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
$reader->setSchema(__DIR__ . '/013.xsd');
while($reader->read() && $reader->nodeType != XMLReader::ELEMENT);
$reader->close();

?>
--EXPECTF--
string(3) "123"
string(3) "456"
===FAIL===

Warning: XMLReader::read(): Element 'foo': %s
