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

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

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
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

array(1) {
  ["foo"]=>
  string(3) "bar"
}
string(3) "bar"
