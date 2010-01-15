--TEST--
Bug #49463 (setAttributeNS fails setting default namespace).
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument('1.0', 'utf-8');
$root = $doc->createElementNS('http://purl.org/rss/1.0/','rdf:RDF');
$doc->appendChild($root);
$root->setAttributeNS("http://www.w3.org/2000/xmlns/","xmlns","http://purl.org/rss/1.0/" );

echo $doc->saveXML()."\n";
?>
--EXPECT--
<?xml version="1.0" encoding="utf-8"?>
<rdf:RDF xmlns:rdf="http://purl.org/rss/1.0/" xmlns="http://purl.org/rss/1.0/"/>
