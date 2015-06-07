--TEST--
Bug #41403 (json_decode() Fatal error: Cannot access property started with '\0')
--SKIPIF--
<?php

if (!extension_loaded('json')) die('skip'); 
?>
--FILE--
<?php

var_dump(json_decode('{"key": {"\u0000": "aa"}}'));
var_dump(json_last_error());
var_dump(json_decode('[{"key1": 0, "\u0000": 1}]'));
var_dump(json_last_error());

echo "Done\n";
?>
--EXPECTF--	
Done
