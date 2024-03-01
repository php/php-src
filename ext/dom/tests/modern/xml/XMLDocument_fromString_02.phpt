--TEST--
DOM\XMLDocument::createFromString 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<?xml version="1.0"?><container/>');
var_dump($dom->saveXMLFile("php://stdout"));

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<container/>
int(52)
