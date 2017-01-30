--TEST--
DOM Document: saveXML with createElement and formatOutput
--CREDITS--
CHU Zhaowei <jhdxr@php.net>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$dom = new domDocument('1.0', 'UTF-8');
$dom->formatOutput = true;

$root = $dom->createElement('root');
$dom->appendChild($root);

$child1 = $dom->createElement('testsuite');
$root->appendChild($child1);

$child11 = $dom->createElement('testcase');
$child11->setAttribute('name', 'leaf1');
$child12 = $dom->createElement('testcase');
$child12->setAttribute('name', 'leaf2');
$child1->appendChild($child11);
$child1->appendChild($child12);

echo $dom->saveXml();
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root>
  <testsuite>
    <testcase name="leaf1"/>
    <testcase name="leaf2"/>
  </testsuite>
</root>