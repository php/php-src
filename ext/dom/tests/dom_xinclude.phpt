--TEST--
Test: Xinclude and Streams
--SKIPIF--
<?php
require_once('skipif.inc');
in_array('compress.zlib', stream_get_wrappers()) or die('skip compress.zlib wrapper is not available');
?>
--FILE--
<?php
$dom = new domdocument;

$data = file_get_contents(__DIR__."/xinclude.xml");
$reldir = str_replace(getcwd(),".",__DIR__);
if (DIRECTORY_SEPARATOR == '\\') {
    $reldir = str_replace('\\',"/", $reldir);
}
$data = str_replace('compress.zlib://ext/dom/tests/','compress.zlib://'.$reldir."/", $data);


$dom->loadXML($data);
$dom->xinclude();
print $dom->saveXML()."\n";
foreach ($dom->documentElement->childNodes as $node) {
    print $node->nodeName."\n";
}
?>
--EXPECTF--
<?xml version="1.0"?>
<foo xmlns:xi="http://www.w3.org/2001/XInclude">
    <book xml:base="compress.zlib://%sbook.xml">
  <title>The Grapes of Wrath</title>
  <author>John Steinbeck</author>
 </book><book xml:base="compress.zlib://%sbook.xml">
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
 </foo>

#text
book
book
#text
