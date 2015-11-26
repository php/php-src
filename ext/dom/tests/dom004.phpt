--TEST--
Test 4: Streams Test
--SKIPIF--
<?php
require_once('skipif.inc');
in_array('compress.zlib', stream_get_wrappers()) or die('skip compress.zlib wrapper is not available');
?>
--FILE--
<?php
$dom = new domdocument;
$dom->load("compress.zlib://".dirname(__FILE__)."/book.xml.gz");
print $dom->saveXML();

--EXPECT--
<?xml version="1.0"?>
<books>
 <book>
  <title>The Grapes of Wrath</title>
  <author>John Steinbeck</author>
 </book>
 <book>
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
</books>
