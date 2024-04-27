--TEST--
Serialize non-default but empty xmlns
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<test xmlns="urn:a"><a:child xmlns:a=" "/></test>');

echo $dom->saveXML(), "\n";

?>
--EXPECTF--
Warning: DOM\XMLDocument::createFromString(): xmlns:a: ' ' is not a valid URI in Entity, line: 1 in %s on line %d
<?xml version="1.0" encoding="UTF-8"?>
<test xmlns="urn:a"><a:child xmlns:a=" "/></test>
