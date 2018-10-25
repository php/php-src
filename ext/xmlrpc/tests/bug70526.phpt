--TEST--
Bug #70526 (xmlrpc_set_type returns false on success)
--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) print "skip";
?>
--FILE--
<?php
$params = date("Ymd\TH:i:s", time());
$rv = xmlrpc_set_type($params, 'datetime');
var_dump($rv);
?>
--EXPECT--
bool(true)
