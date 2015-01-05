--TEST--
SERVER_PROTOCOL header availability
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_SERVER["SERVER_PROTOCOL"]);');

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
GET / HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		echo fgets($fp);
	}
}

fclose($fp);

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}


if(fwrite($fp, <<<HEADER
GET / HTTP/1.0
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		echo fgets($fp);
	}
}

fclose($fp);
?>
--EXPECTF--
HTTP/1.1 200 OK
Host: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

string(8) "HTTP/1.1"
HTTP/1.0 200 OK
Host: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

string(8) "HTTP/1.0"
