--TEST--
Bug #73246 (XMLReader: encoding length not checked)
--EXTENSIONS--
xmlreader
--FILE--
<?php
$reader = new XMLReader();
$reader->open(__FILE__, "UTF\0-8");
$reader->XML('<?xml version="1.0"?><root/>', "UTF\0-8");
?>
--EXPECTF--
Warning: XMLReader::open(): Encoding must not contain NUL bytes in %s on line %d

Warning: XMLReader::XML(): Encoding must not contain NUL bytes in %s on line %d
