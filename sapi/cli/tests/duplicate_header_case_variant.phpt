--TEST--
apache_request_headers()/getallheaders() survive a duplicate header whose name only differs by case
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start("var_dump(apache_request_headers());");

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

fwrite($fp, "GET / HTTP/1.1\r\nHost: $host\r\nX-Foo: a\r\nx-foo: b\r\n\r\n");
fflush($fp);
while (!feof($fp)) {
    echo fgets($fp);
}
fclose($fp);
?>
--EXPECTF--
HTTP/1.1 200 OK
%a
array(3) {
  ["Host"]=>
  string(%d) "%s"
  ["X-Foo"]=>
  string(1) "a"
  ["x-foo"]=>
  string(4) "a, b"
}
