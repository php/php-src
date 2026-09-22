--TEST--
XML parsing with LIBXML_NO_XXE
--EXTENSIONS--
simplexml
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

var_dump(simplexml_load_string($xml, options: LIBXML_NOENT | LIBXML_NO_XXE));

?>
--EXPECT--
object(SimpleXMLElement)#1 (1) {
  ["foo"]=>
  string(3) "bar"
}
