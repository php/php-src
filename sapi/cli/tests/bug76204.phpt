--TEST--
CONNECT method is partially supported
--SKIPIF--
<?php include 'skipif.inc' ?>
--FILE--
<?php
include 'php_cli_server.inc';

php_cli_server_start();

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port) ?: 80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
    die("connect failed");
}

if (fwrite($fp, "CONNECT www.example.com:443 HTTP/1.1\r\n\r\n")) {
    echo fgets($fp);
}
?>
===DONE===
--EXPECT--
HTTP/1.1 501 Not Implemented
===DONE===
