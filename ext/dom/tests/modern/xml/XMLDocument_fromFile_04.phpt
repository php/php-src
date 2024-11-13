--TEST--
Dom\XMLDocument::createFromFile 04
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromFile(__DIR__."/../../book.xml");
echo $dom->saveXml();

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
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
