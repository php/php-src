--TEST--
Cloning a dtd node
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html></html>');

$dt1 = clone $dom->doctype;
var_dump($dt1 === $dom->doctype);
var_dump($dt1->name);

echo "\n";

$dt2 = $dom->doctype->cloneNode();
var_dump($dt2 === $dt1);
var_dump($dt2 === $dom->doctype);
var_dump($dt2->name);

?>
--EXPECT--
bool(false)
string(4) "html"

bool(false)
bool(false)
string(4) "html"
