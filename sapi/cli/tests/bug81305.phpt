--TEST--
Bug #81305 (Built-in Webserver Drops Requests With "Upgrade" Header)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start();

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if (fwrite($fp, <<<HEADER
GET / HTTP/1.1
Host: {$host}
Upgrade: HTTP/2.0
Connection: upgrade


HEADER)) {
	fpassthru($fp);
}

fclose($fp);
?>
--EXPECTF--
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

Hello world
