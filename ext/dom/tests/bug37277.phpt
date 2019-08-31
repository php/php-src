--TEST--
Bug #37277 (cloning Dom Documents or Nodes does not work)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$dom1 = new DomDocument('1.0', 'UTF-8');

$xml = '<foo />';
$dom1->loadXml($xml);

$node = clone $dom1->documentElement;

$dom2 = new DomDocument('1.0', 'UTF-8');
$dom2->appendChild($dom2->importNode($node->cloneNode(true), TRUE));

print $dom2->saveXML();


?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<foo/>
