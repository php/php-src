--TEST--
Test 11: php:function Support
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
print "Test 11: php:function Support\n";

$dom = new domDocument();
  $dom->load(dirname(__FILE__)."/xslt011.xsl");
  $proc = new xsltprocessor;
  $xsl = $proc->importStylesheet($dom);
  
  $xml = new DomDocument();
  $xml->load(dirname(__FILE__)."/xslt011.xml");
  
  print $proc->transformToXml($xml);
  function foobar($id ) {
    if (is_array($id)) {
        return $id[0]->value;
    } else {
        return $id;
    }
  }
--EXPECT--
Test 11: php:function Support
<?xml version="1.0"?>
foobar
foobar
