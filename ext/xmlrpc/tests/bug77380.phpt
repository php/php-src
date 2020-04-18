--TEST--
Bug #77380 (Global out of bounds read in xmlrpc base64 code)
--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) print "skip";
?>
--FILE--
<?php
var_dump(xmlrpc_decode(base64_decode("PGJhc2U2ND7CkzwvYmFzZTY0Pgo=")));
?>
--EXPECT--
object(stdClass)#1 (2) {
  ["scalar"]=>
  string(0) ""
  ["xmlrpc_type"]=>
  string(6) "base64"
}
