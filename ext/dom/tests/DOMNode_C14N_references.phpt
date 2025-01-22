--TEST--
Test: Canonicalization - C14N() with references
--EXTENSIONS--
dom
--FILE--
<?php
// Adapted from canonicalization.phpt

$xml = <<<EOXML
<?xml version="1.0" encoding="ISO-8859-1" ?>
<foo xmlns="http://www.example.com/ns/foo"
     xmlns:fubar="http://www.example.com/ns/fubar" xmlns:test="urn::test"><contain>
  <bar><test1 /></bar>
  <bar><test2 /></bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test3 /></fubar:bar>
  <fubar:bar><test4 /></fubar:bar>
</contain>
</foo>
EOXML;

$dom = new DOMDocument();
$dom->loadXML($xml);
$doc = $dom->documentElement->firstChild;

$xpath = [
    'query' => '(//a:contain | //a:bar | .//namespace::*)',
    'namespaces' => ['a' => 'http://www.example.com/ns/foo'],
];
$prefixes = ['test'];

foreach ($xpath['namespaces'] as $k => &$v);
unset($v);
foreach ($xpath as $k => &$v);
unset($v);
foreach ($prefixes as $k => &$v);
unset($v);

echo $doc->C14N(true, false, $xpath, $prefixes);
?>
--EXPECT--
<contain xmlns="http://www.example.com/ns/foo" xmlns:test="urn::test"><bar></bar><bar></bar></contain>
