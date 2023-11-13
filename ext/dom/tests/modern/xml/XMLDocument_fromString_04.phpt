--TEST--
DOM\XMLDocument::createFromString 04
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<?xml version="1.0" encoding="Windows-1251"?><container/>');
var_dump($dom->encoding);
echo $dom->saveXML();

?>
--EXPECT--
string(12) "Windows-1251"
<?xml version="1.0" encoding="Windows-1251"?>
<container/>
