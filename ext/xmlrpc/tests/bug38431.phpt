--TEST--
Bug #38431 (xmlrpc_get_type() crashes PHP on objects)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

var_dump(xmlrpc_get_type(new stdclass));
var_dump(xmlrpc_get_type(array()));
$var = array(1,2,3);
var_dump(xmlrpc_get_type($var));
$var = array("test"=>1,2,3);
var_dump(xmlrpc_get_type($var));
$var = array("test"=>1,"test2"=>2);
var_dump(xmlrpc_get_type($var));

echo "Done\n";
?>
--EXPECT--
string(5) "array"
string(5) "array"
string(5) "array"
string(5) "mixed"
string(6) "struct"
Done
