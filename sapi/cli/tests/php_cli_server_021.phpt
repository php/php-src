--TEST--
PATH_INFO test with -P / --redirect
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
echo "-- Without redirect\n";
$proc_handle = php_cli_server_start('var_dump($_SERVER["PATH_INFO"]);', false);
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS);
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . '/foo/bar');
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . '/foo-1.1/bar');
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . '/foo/bar.png');
php_cli_server_stop($proc_handle);

echo "-- With redirect\n";
$proc_handle = php_cli_server_start('var_dump($_SERVER["PATH_INFO"]);', false, '--redirect');
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS);
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . '/foo/bar');
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . '/foo-1.1/bar');
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . '/foo/bar.png');
php_cli_server_stop($proc_handle);
?>
Done
--EXPECTF--	
-- Without redirect
NULL
string(8) "/foo/bar"

Warning: file_get_contents(%s/foo-1.1/bar): failed to open stream: HTTP request failed! HTTP/1.0 404 Not Found
 in %s on line %d

Warning: file_get_contents(%s/foo/bar.png): failed to open stream: HTTP request failed! HTTP/1.0 404 Not Found
 in %s on line %d
-- With redirect
NULL
string(8) "/foo/bar"
string(12) "/foo-1.1/bar"
string(12) "/foo/bar.png"
Done
