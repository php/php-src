--TEST--
XMLReader: accessing empty and non existing attributes 
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php 
/* $Id$ */

$xmlstring =<<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<foo bar=""/>
EOF;

$reader = new XMLReader();
$reader->XML($xmlstring);
$reader->read();
var_dump($reader->getAttribute('bar'));
var_dump($reader->getAttribute('baz'));
$reader->close();

$xmlstring =<<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE foo SYSTEM "012.dtd">
<foo bar=""/>
EOF;

$xmlstring = str_replace('012.dtd', dirname(__FILE__).'/012.dtd', $xmlstring);

$reader = new XMLReader();
$reader->XML($xmlstring);
$reader->setParserProperty(XMLReader::DEFAULTATTRS, true);
while($reader->read() && $reader->nodeType != XMLReader::ELEMENT);
var_dump($reader->getAttribute('bar'));
var_dump($reader->getAttribute('baz'));
$reader->close();

?>
===FILE===
<?php

$reader = new XMLReader();
$reader->open(dirname(__FILE__) . '/012.xml');
//$reader->setParserProperty(XMLReader::DEFAULTATTRS, true);
while($reader->read() && $reader->nodeType != XMLReader::ELEMENT);
var_dump($reader->getAttribute('bar'));
var_dump($reader->getAttribute('baz'));
$reader->close();

$reader = new XMLReader();
$reader->open(dirname(__FILE__) . '/012.xml');
$reader->setParserProperty(XMLReader::DEFAULTATTRS, true);
while($reader->read() && $reader->nodeType != XMLReader::ELEMENT);
var_dump($reader->getAttribute('bar'));
var_dump($reader->getAttribute('baz'));
$reader->close();

?>
===DONE===
--EXPECT--
string(0) ""
NULL
string(0) ""
string(0) ""
===FILE===
string(0) ""
NULL
string(0) ""
string(0) ""
===DONE===
--UEXPECT--
unicode(0) ""
NULL
unicode(0) ""
unicode(0) ""
===FILE===
unicode(0) ""
NULL
unicode(0) ""
unicode(0) ""
===DONE===
