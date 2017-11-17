--TEST--
Implement Req #65917 (getallheaders() is not supported by the built-in web server)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
header('Bar-Foo: Foo');
var_dump(getallheaders());
var_dump(apache_request_headers());
var_dump(apache_response_headers());
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
Host: {$host}
Foo-Bar: Bar


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
Date: %s
Connection: close
X-Powered-By: %s
Bar-Foo: Foo
Content-type: text/html; charset=UTF-8

array(2) {
  ["Host"]=>
  string(9) "localhost"
  ["Foo-Bar"]=>
  string(3) "Bar"
}
array(2) {
  ["Host"]=>
  string(9) "localhost"
  ["Foo-Bar"]=>
  string(3) "Bar"
}
array(3) {
  ["X-Powered-By"]=>
  string(%d) "P%s"
  ["Bar-Foo"]=>
  string(3) "Foo"
  ["Content-type"]=>
  string(24) "text/html; charset=UTF-8"
}
