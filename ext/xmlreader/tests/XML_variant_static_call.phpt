--TEST--
XMLReader: XML variant static call
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip XMLReader extension required"; ?>
--FILE--
<?php

$xml = '<?xml version="1.0" encoding="UTF-8"?><books></books>';

$reader = XMLReader::XML($xml);
while ($reader->read()) {
  echo $reader->name."\n";
}
?>
--EXPECTF--
Deprecated: Non-static method XMLReader::XML() should not be called statically in %s on line %d
books
books
