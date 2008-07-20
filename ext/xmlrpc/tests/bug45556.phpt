--TEST--
Bug #45556 (Return value from callback isn't freed)
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
expat reports error code 5
	description: Invalid document end
	line: 1
	column: 1
	byte index: 0
	total bytes: 0

	data beginning 0 before byte index: foo
 
Warning: xmlrpc_server_call_method(): xml parse error: [line 1, column 1, message: Invalid document end] Unable to add introspection data returned from bar::test() in %s on line %d

Warning: xmlrpc_server_call_method(): Invalid callback 'foo::bar' passed in %s on line %d
