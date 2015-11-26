--TEST--
Bug #45556 (Return value from callback isn't freed)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

$options = array ();
$request = xmlrpc_encode_request ("system.describeMethods", $options);
$server = xmlrpc_server_create ();


function foo() { return 11111; }

class bar {
	static public function test() {
		return 'foo';
	}
}

xmlrpc_server_register_introspection_callback($server, 'foobar');
xmlrpc_server_register_introspection_callback($server, array('bar', 'test'));
xmlrpc_server_register_introspection_callback($server, array('foo', 'bar'));

$options = array ('output_type' => 'xml', 'version' => 'xmlrpc');
xmlrpc_server_call_method ($server, $request, NULL, $options);

?>
--EXPECTF--
Warning: xmlrpc_server_call_method(): Invalid callback 'foobar' passed in %s on line %d

Warning: xmlrpc_server_call_method(): xml parse error: [line 1, column 1, message: Invalid document end] Unable to add introspection data returned from bar::test() in %s on line %d

Warning: xmlrpc_server_call_method(): Invalid callback 'foo::bar' passed in %s on line %d
