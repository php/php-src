--TEST--
DOM\XMLDocument::createFromFile 04
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromFile(__DIR__."/../../book.xml");
echo $dom->saveXML();

?>
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
