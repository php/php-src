--TEST--
Bug #35785 (SimpleXML memory read error)
--FILE--
<?php

$options["database"] = "xmldatabase";
$xml = simplexml_load_string("<root></root>");
$count = count($xml->posts) + 1;
$xml->bla->posts[$count]->name = $_POST["name"];
echo $xml->asXML();
?>
===DONE===
<?php exit(0); __halt_compiler(); ?>
--EXPECTF--

Notice: Undefined index:  name in %sbug35785.phpt on line %d

Strict Standards: Creating default object from empty value in %sbug35785.phpt on line %d

Warning: Attempt to assign property of non-object in %sbug35785.phpt on line %d
<?xml version="1.0"?>
<root/>
