--TEST--
Bug #45555 (Segfault with invalid non-string as register_introspection_callback)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

$options = array ();
$request = xmlrpc_encode_request ("system.describeMethods", $options);
$server = xmlrpc_server_create ();

xmlrpc_server_register_introspection_callback($server, 1);
xmlrpc_server_register_introspection_callback($server, array('foo', 'bar'));

$options = array ('output_type' => 'xml', 'version' => 'xmlrpc');
xmlrpc_server_call_method ($server, $request, NULL, $options);

?>
--EXPECTF--
Warning: xmlrpc_server_call_method(): Invalid callback '1' passed in %s on line %d

Warning: xmlrpc_server_call_method(): Invalid callback 'foo::bar' passed in %s on line %d
