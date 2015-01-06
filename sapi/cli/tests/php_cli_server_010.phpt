--TEST--
Bug #60180 ($_SERVER["PHP_SELF"] incorrect)
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_SERVER["PHP_SELF"], $_SERVER["SCRIPT_NAME"], $_SERVER["PATH_INFO"], $_SERVER["QUERY_STRING"]);', null);

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
GET /foo/bar?foo=bar HTTP/1.1
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
GET /index.php/foo/bar/?foo=bar HTTP/1.0
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

string(18) "/index.php/foo/bar"
string(10) "/index.php"
string(8) "/foo/bar"
string(7) "foo=bar"
HTTP/1.0 200 OK
Host: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

string(19) "/index.php/foo/bar/"
string(10) "/index.php"
string(9) "/foo/bar/"
string(7) "foo=bar"
