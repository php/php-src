--TEST--
Bug #72155 (use-after-free caused by get_zval_xmlrpc_type)
--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) print "skip";
?>
--FILE--
<?php
$var0 = fopen("/etc/passwd","r");
$var1 = xmlrpc_encode($var0);
var_dump($var1);
?>
--EXPECTF--
string(109) "<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <int>5</int>
 </value>
</param>
</params>
"
