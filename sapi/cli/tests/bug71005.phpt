--TEST--
Bug #71005 (Segfault in php_cli_server_dispatch_router())
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php

$code = <<<'EOF'
set_exception_handler(function () { echo 'goodbye'; });
throw new Exception;
EOF;

include "php_cli_server.inc";
php_cli_server_start($code);

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port) ?: 80;

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
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

goodbye
