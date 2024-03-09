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

// This should fail because it has been imported already above in legacy DOM
try {
    DOM\import_simplexml($s);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
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
DOM\import_simplexml(): Argument #1 ($node) must not be already imported as a DOMNode
