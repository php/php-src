--TEST--
CLI server does not merge pipelined requests into one request
--SKIPIF--
<?php
include 'skipif.inc';
?>
--FILE--
<?php
include 'php_cli_server.inc';
php_cli_server_start('echo "[", $_SERVER["REQUEST_URI"], "|", $_SERVER["QUERY_STRING"], "]";');

$socket = php_cli_server_connect();
fwrite($socket, "GET /first.php?one HTTP/1.1\r\nHost: localhost\r\n\r\n"
    . "GET /second.php?two HTTP/1.1\r\nHost: localhost\r\n\r\n");
$response = stream_get_contents($socket);
fclose($socket);

preg_match_all('/\[([^]]*)\]/', $response, $matches);
var_dump(array_diff($matches[1], ['/first.php?one|one', '/second.php?two|two']));
var_dump(file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/third.php?three'));
?>
--EXPECT--
array(0) {
}
string(24) "[/third.php?three|three]"
