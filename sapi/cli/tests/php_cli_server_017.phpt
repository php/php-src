--TEST--
Implement Req #60850 (Built in web server does not set $_SERVER['SCRIPT_FILENAME'] when using router)
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
var_dump($_SERVER['SCRIPT_FILENAME']);
PHP
);

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
POST / HTTP/1.1
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
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

string(%d) "%sindex.php"
