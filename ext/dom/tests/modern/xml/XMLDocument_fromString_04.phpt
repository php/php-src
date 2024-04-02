--TEST--
DOM\XMLDocument::createFromString 04
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (getenv('SKIP_ASAN') && LIBXML_VERSION < 20911) die('xleak leaks under libxml2 versions older than 2.9.11');
?>
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<?xml version="1.0" encoding="Windows-1251"?><container/>');
var_dump($dom->charset);
echo $dom->saveXML();

?>
--EXPECT--
string(12) "Windows-1251"
<?xml version="1.0" encoding="Windows-1251"?>
<container/>
