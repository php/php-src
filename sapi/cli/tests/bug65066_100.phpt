--TEST--
Bug #65066 (Cli server not responsive when responding with 422 http status code): 100 status code
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('http_response_code(100);');

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
?>
--EXPECTF--
HTTP/1.1 100 Continue
Host: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html
