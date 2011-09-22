--TEST--
Bug #55755 (SegFault when outputting header WWW-Authenticate)
--SKIPIF--
<?php
include "skipif.inc"; 
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die ("skip not for Windows");
}
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_SERVER["PHP_AUTH_USER"], $_SERVER["PHP_AUTH_PW"]);');

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
GET / HTTP/1.1
Host: {$host}
Authorization: Basic Zm9vOmJhcg==


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
Connection: closed
X-Powered-By: PHP/%s-dev
Content-type: text/html

string(3) "foo"
string(3) "bar"
