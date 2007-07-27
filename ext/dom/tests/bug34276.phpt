--TEST--
Bug #34276 setAttributeNS and default namespace
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$xml = <<<HERE
<?xml version="1.0" encoding="ISO-8859-1" ?>
<foo xmlns="http://www.example.com/ns/foo"
     xmlns:fubar="http://www.example.com/ns/fubar" attra="attra" />
HERE;

function dump($elems) {
	foreach ($elems as $elem) {
		var_dump($elem->nodeName);
		dump($elem->childNodes);
	}
}

$dom = new DOMDocument();
$dom->loadXML($xml);
$foo = $dom->documentElement;
var_dump($foo->hasAttributeNS('http://www.example.com/ns/foo', 'attra'));
var_dump($foo->getAttributeNS('http://www.example.com/ns/foo', 'attra'));

$foo->setAttributeNS('http://www.example.com/ns/foo', 'attra', 'attranew');
$foo->setAttributeNS('http://www.example.com/ns/fubar', 'attrb', 'attrbnew');
$foo->setAttributeNS('http://www.example.com/ns/foo', 'attrc', 'attrc');

var_dump($foo->getAttributeNS('http://www.example.com/ns/foo', 'attra'));
var_dump($foo->getAttributeNS('http://www.example.com/ns/fubar', 'attrb'));
var_dump($foo->getAttributeNS('http://www.example.com/ns/foo', 'attrc'));

print $dom->saveXML();
?>
--EXPECT--
bool(false)
string(0) ""
string(8) "attranew"
string(8) "attrbnew"
string(5) "attrc"
<?xml version="1.0" encoding="ISO-8859-1"?>
<foo xmlns="http://www.example.com/ns/foo" xmlns:fubar="http://www.example.com/ns/fubar" xmlns:default="http://www.example.com/ns/foo" attra="attra" default:attra="attranew" fubar:attrb="attrbnew" default:attrc="attrc"/>
