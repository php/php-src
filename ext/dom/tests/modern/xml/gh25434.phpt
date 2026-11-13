--TEST--
GH-25434 (C14NFile() on a Dom\XMLDocument leaves the document changed and leaks memory)
--CREDITS--
Wonseok Oh (marinelay)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = Dom\XMLDocument::createFromString('<root xmlns:p="urn:p"><p:a p:x="1"/></root>');
var_dump($doc->C14NFile(__DIR__ . '/gh25434.xml'));
echo file_get_contents(__DIR__ . '/gh25434.xml'), "\n";
echo $doc->saveXml(), "\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25434.xml');
?>
--EXPECT--
int(48)
<root xmlns:p="urn:p"><p:a p:x="1"></p:a></root>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:p="urn:p"><p:a p:x="1"/></root>
