--TEST--
Test: Xinclude and Streams
--SKIPIF--
<?php  require_once('skipif.inc'); ?>
--FILE--
<?php
$dom = new domdocument;
$dom->load(dirname(__FILE__)."/xinclude.xml");
$dom->xinclude();
print $dom->saveXML();

--EXPECT--
<?xml version="1.0"?>
<foo xmlns:xi="http://www.w3.org/2001/XInclude">
    <book xml:base="compress.zlib://ext/dom/tests/book.xml">
  <title>The Grapes of Wrath</title>
  <author>John Steinbeck</author>
 </book><book xml:base="compress.zlib://ext/dom/tests/book.xml">
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
 </foo>
