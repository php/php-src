--TEST--
defining INI options with -d
--SKIPIF--
<?php 
include "skipif.inc"; 
if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`$php -n -d max_execution_time=111 -r 'var_dump(ini_get("max_execution_time"));'`);
var_dump(`$php -n -d max_execution_time=500 -r 'var_dump(ini_get("max_execution_time"));'`);
var_dump(`$php -n -d max_execution_time=500 -d max_execution_time=555 -r 'var_dump(ini_get("max_execution_time"));'`);
var_dump(`$php -n -d upload_tmp_dir=/test/path -d max_execution_time=555 -r 'var_dump(ini_get("max_execution_time")); var_dump(ini_get("upload_tmp_dir"));'`);

echo "Done\n";
?>
--EXPECTF--	
string(16) "string(3) "111"
"
string(16) "string(3) "500"
"
string(16) "string(3) "555"
"
string(40) "string(3) "555"
string(10) "/test/path"
"
Done
