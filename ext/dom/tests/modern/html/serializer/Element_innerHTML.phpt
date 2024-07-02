--TEST--
Test reading Element::$innerHTML on HTML documents
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><head><title>Test</title></head><body><div></div><p>Hello, World!</p></body></html>');
var_dump($dom->getElementsByTagName('body')[0]->innerHTML);
var_dump($dom->getElementsByTagName('head')[0]->innerHTML);
var_dump($dom->getElementsByTagName('html')[0]->innerHTML);
var_dump($dom->getElementsByTagName('div')[0]->innerHTML);
var_dump($dom->getElementsByTagName('p')[0]->innerHTML);

?>
--EXPECT--
string(31) "<div></div><p>Hello, World!</p>"
string(19) "<title>Test</title>"
string(76) "<head><title>Test</title></head><body><div></div><p>Hello, World!</p></body>"
string(0) ""
string(13) "Hello, World!"
