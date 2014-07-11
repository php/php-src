--TEST--
Bug #61264: xmlrpc_parse_method_descriptions leaks temporary variable
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php
$xml = <<<XML
<?xml version="1.0" encoding="utf-8"?>
<a>
 <b>foo</b>
</a>
XML;
var_dump(xmlrpc_parse_method_descriptions($xml));
?>
--EXPECT--
array(1) {
  ["b"]=>
  string(3) "foo"
}
