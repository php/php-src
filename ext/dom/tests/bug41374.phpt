--TEST--
Bug #41374 (wholetext concats values of wrong nodes)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$xml = <<<EOXML
<?xml version="1.0" encoding="ISO-8859-1" ?>
<root>foo<child />baz</root>
EOXML;

$doc = new DOMDocument();
$doc->loadXML($xml);

$root = $doc->documentElement;
$foo = $root->firstChild;

var_dump($foo->wholeText == "foo");

$bar = $root->insertBefore($doc->createTextNode("bar"), $foo->nextSibling);

var_dump($foo->wholeText == "foobar");
var_dump($foo->wholeText == $bar->wholeText);
$baz = $bar->nextSibling->nextSibling;

var_dump($baz->wholeText === $foo->wholeText);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
