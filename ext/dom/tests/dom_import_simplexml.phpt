--TEST--
Interop Test: Import from SimpleXML
--EXTENSIONS--
dom
simplexml
--FILE--
<?php
$s = simplexml_load_file(__DIR__."/book.xml");
if(!$s) {
  echo "Error while loading the document\n";
  exit;
}
$dom = dom_import_simplexml($s);
print $dom->ownerDocument->saveXML();
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
