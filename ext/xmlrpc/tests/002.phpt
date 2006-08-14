--TEST--
xmlrpc_encode_request() and various arguments
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

$r = xmlrpc_encode_request("method", array());
var_dump(xmlrpc_decode_request($r, $method));
var_dump($method);

$r = xmlrpc_encode_request("method", 1);
var_dump(xmlrpc_decode_request($r, $method));
var_dump($method);

$r = xmlrpc_encode_request("method", 'param');
var_dump(xmlrpc_decode_request($r, $method));
var_dump($method);

$r = xmlrpc_encode_request(-1, "");
var_dump(xmlrpc_decode_request($r, $method));
var_dump($method);

$r = xmlrpc_encode_request(array(), 1);
var_dump(xmlrpc_decode_request($r, $method));
var_dump($method);

echo "Done\n";
?>
--EXPECTF--	
array(0) {
}
string(6) "method"
array(1) {
  [0]=>
  int(1)
}
string(6) "method"
array(1) {
  [0]=>
  string(5) "param"
}
string(6) "method"
array(1) {
  [0]=>
  string(0) ""
}
string(2) "-1"

Notice: Array to string conversion in %s on line %d
array(1) {
  [0]=>
  int(1)
}
string(5) "Array"
Done
