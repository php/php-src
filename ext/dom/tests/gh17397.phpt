--TEST--
GH-17397 (Assertion failure ext/dom/php_dom.c)
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createFromString('<!DOCTYPE html><html></html>');
var_dump($dom->doctype->prefix);
echo $dom->saveXml();
?>
--EXPECTF--
Warning: Undefined property: Dom\DocumentType::$prefix in %s on line %d
NULL
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body></body></html>
