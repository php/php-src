--TEST--
Bug #79451 (Using DOMDocument->replaceChild on doctype causes double free)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$dom = new \DOMDocument();
$dom->loadHTML("<!DOCTYPE html><p>hello</p>");
$impl = new \DOMImplementation();
$dt = $impl->createDocumentType("html_replace", "", "");
$dom->replaceChild($dt, $dom->doctype);

var_dump($dom->doctype->name);
echo $dom->saveXML();
?>
--EXPECTF--
string(12) "html_replace"
<?xml version="1.0" standalone="yes"?>
<!DOCTYPE html_replace>
<html><body><p>hello</p></body></html>
