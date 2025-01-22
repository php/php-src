--TEST--
Dom\XMLDocument::createFromString 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<?xml version="1.0"?><container/>');
var_dump($dom->saveXmlFile(__DIR__ . "/fromString_02.xml"));
echo file_get_contents(__DIR__ . "/fromString_02.xml");

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/fromString_02.xml");
?>
--EXPECT--
int(51)
<?xml version="1.0" encoding="UTF-8"?>
<container/>
