--TEST--
Test flag LIBXML_NO_XXE
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (!defined('LIBXML_NO_XXE')) die('skip LIBXML_NO_XXE not available');
?>
--FILE--
<?php
$xml = <<< XML
<?xml version='1.0' encoding='utf-8'?>
<!DOCTYPE set [
    <!ENTITY foo '<foo>bar</foo>'>
    <!ENTITY xxe SYSTEM "file:///etc/passwd">
]>
<set>&foo;&xxe;</set>
XML;

$doc = Dom\XMLDocument::createFromString($xml, LIBXML_NOENT | LIBXML_NO_XXE);
echo $doc->saveXML();
?>
--EXPECT--
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE set [
<!ENTITY foo "<foo>bar</foo>">
<!ENTITY xxe SYSTEM "file:///etc/passwd">
]>
<set><foo>bar</foo></set>
