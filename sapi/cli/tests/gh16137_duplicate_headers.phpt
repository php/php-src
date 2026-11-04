--TEST--
GH-16137: CLI server handling of repeated request headers
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('$h = getallheaders(); unset($h["Host"], $h["Connection"]); var_dump($h);');
$ctx = stream_context_create(['http' => ['header' => [
    'X-Foo: a',
    'X-Foo: b',
    'Cookie: Foo',
    'cOOKIE: Bar',
    'Set-Cookie: first',
    'set-cookie: second',
]]]);
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS, false, $ctx);
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS);
?>
--EXPECT--
array(5) {
  ["X-Foo"]=>
  string(4) "a, b"
  ["Cookie"]=>
  string(3) "Foo"
  ["cOOKIE"]=>
  string(8) "Foo, Bar"
  ["Set-Cookie"]=>
  string(5) "first"
  ["set-cookie"]=>
  string(6) "second"
}
array(0) {
}
