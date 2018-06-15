--TEST--
Bug #61097 (Memory leak in xmlrpc functions copying zvals)
--SKIPIF--
<?php if(!extension_loaded("xmlrpc")) die("skip xmlrpc extension not loaded"); ?>
--FILE--
<?php
$server = xmlrpc_server_create();

$method = 'abc';
xmlrpc_server_register_introspection_callback($server, $method);
xmlrpc_server_register_method($server, 'abc', $method);

echo 'Done';
?>
--EXPECT--
Done
