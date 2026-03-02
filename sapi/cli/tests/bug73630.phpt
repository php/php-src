--TEST--
Bug #73630 (Built-in Weberver - overwrite $_SERVER['request_uri'])
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$code = <<<'EOF'
var_dump(strncmp($_SERVER['REQUEST_URI'], "/overflow.php", strlen("/overflow.php")));
var_dump(strlen($_SERVER['QUERY_STRING']));
EOF;

include "php_cli_server.inc";
php_cli_server_start($code);

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

$path = "/overflow.php?" . str_repeat("x", 16400) . "//example.com";

if (fwrite($fp, <<<HEADER
GET $path HTTP/1.1
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

int(0)
int(16413)
