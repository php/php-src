--TEST--
Bug #50285 (xmlrpc does not preserve keys in encoded indexed arrays)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

function test1($func, $params) {
    return array(1=>'One', 3=>'Three', 5=>'Five');
}

function test2($func, $params) {
    return array('One', 'Three', 'Five', 5);
}

function test3($func, $params) {
    return array('One', 3 => 'Three', b'Five' => 5, 'Six');
}

function test4($func, $params) {
    return array('One', 'Three', 'Five', b'Six' => 6);
}

$server = xmlrpc_server_create();
$result = xmlrpc_server_register_method($server, 'test1', 'test1');
$HTTP_RAW_POST_DATA = <<<EOD
<?xml version="1.0" encoding="UTF-8"?>
<methodCall>
<methodName>test1</methodName>
<params />
</methodCall>
EOD;
$response = xmlrpc_server_call_method($server, $HTTP_RAW_POST_DATA, null);
var_dump(xmlrpc_decode($response));

// ------------ 

$server = xmlrpc_server_create();
$result = xmlrpc_server_register_method($server, 'test2', 'test2');
$HTTP_RAW_POST_DATA = <<<EOD
<?xml version="1.0" encoding="UTF-8"?>
<methodCall>
<methodName>test2</methodName>
<params />
</methodCall>
EOD;
$response = xmlrpc_server_call_method($server, $HTTP_RAW_POST_DATA, null);
var_dump(xmlrpc_decode($response));

// ------------ 

$server = xmlrpc_server_create();
$result = xmlrpc_server_register_method($server, 'test3', 'test3');
$HTTP_RAW_POST_DATA = <<<EOD
<?xml version="1.0" encoding="UTF-8"?>
<methodCall>
<methodName>test3</methodName>
<params />
</methodCall>
EOD;
$response = xmlrpc_server_call_method($server, $HTTP_RAW_POST_DATA, null);
var_dump(xmlrpc_decode($response));

// ------------ 

$server = xmlrpc_server_create();
$result = xmlrpc_server_register_method($server, 'test4', 'test4');
$HTTP_RAW_POST_DATA = <<<EOD
<?xml version="1.0" encoding="UTF-8"?>
<methodCall>
<methodName>test4</methodName>
<params />
</methodCall>
EOD;
$response = xmlrpc_server_call_method($server, $HTTP_RAW_POST_DATA, null);
var_dump(xmlrpc_decode($response));

?>
--EXPECT--
array(3) {
  [1]=>
  string(3) "One"
  [3]=>
  string(5) "Three"
  [5]=>
  string(4) "Five"
}
array(4) {
  [0]=>
  string(3) "One"
  [1]=>
  string(5) "Three"
  [2]=>
  string(4) "Five"
  [3]=>
  int(5)
}
array(4) {
  [0]=>
  string(3) "One"
  [3]=>
  string(5) "Three"
  ["Five"]=>
  int(5)
  [4]=>
  string(3) "Six"
}
array(4) {
  [0]=>
  string(3) "One"
  [1]=>
  string(5) "Three"
  [2]=>
  string(4) "Five"
  ["Six"]=>
  int(6)
}
