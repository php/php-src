--TEST--
Bug #70728 (Type Confusion Vulnerability in PHP_to_XMLRPC_worker)
--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) print "skip";
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
$obj = new stdClass;
$obj->xmlrpc_type = 'base64';
$obj->scalar = 0x1122334455;
var_dump(xmlrpc_encode($obj));
var_dump($obj);
?>
--EXPECTF--	
string(135) "<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <base64>NzM1ODgyMjkyMDU=&#10;</base64>
 </value>
</param>
</params>
"
object(stdClass)#1 (2) {
  ["xmlrpc_type"]=>
  string(6) "base64"
  ["scalar"]=>
  int(73588229205)
}
