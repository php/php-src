--TEST--
Bug #42139 (XMLReader option constants are broken using XML())
--EXTENSIONS--
xmlreader
--FILE--
<?php

$xml = <<<XML
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE root [
<!ELEMENT root ANY>
<!ENTITY x "y">
]>
<root>&x;</root>
XML;

$reader = new XMLReader;
$reader->XML( $xml, NULL, LIBXML_NOENT);
while ( $reader->read() ) {
  echo "{$reader->nodeType}, {$reader->name}, {$reader->value}\n";
}
$reader->close();

?>
--EXPECT--
10, root, 
1, root, 
3, #text, y
15, root, 
