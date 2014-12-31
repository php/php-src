--TEST--
Bug #65633 (built-in server treat some http headers as case-sensitive)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
var_dump($_COOKIE, $_SERVER['HTTP_FOO']);
PHP
);

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
GET / HTTP/1.1
cookie: foo=bar
foo: bar


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
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

array(1) {
  ["foo"]=>
  string(3) "bar"
}
string(3) "bar"
