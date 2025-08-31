--TEST--
GH-11791 (Wrong default value of DOMDocument.xmlStandalone)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root/>');
var_dump($doc->xmlStandalone);
$doc->xmlStandalone = true;
var_dump($doc->xmlStandalone);

$doc = new DOMDocument();
$doc->loadXML('<?xml version="1.0"?><root/>');
var_dump($doc->xmlStandalone);
$doc->xmlStandalone = true;
var_dump($doc->xmlStandalone);

$doc = new DOMDocument();
$doc->loadXML('<?xml version="1.0" standalone="no"?><root/>');
var_dump($doc->xmlStandalone);
$doc->xmlStandalone = true;
var_dump($doc->xmlStandalone);

$doc = new DOMDocument();
$doc->loadXML('<?xml version="1.0" standalone="yes"?><root/>');
var_dump($doc->xmlStandalone);
$doc->xmlStandalone = false;
var_dump($doc->xmlStandalone);
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
