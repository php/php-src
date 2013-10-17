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
header('Content-Type: text/plain');
var_dump(getallheaders());
var_dump(apache_request_headers());
var_dump(apache_response_headers());
PHP
);

$opts = array(
    'http'=>array(
        'method'=>"GET",
        'header'=>"Foo-Bar: bar\r\n"
    )
);

$context = stream_context_create($opts);
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS, false, $context);
?>
--EXPECTF--
array(2) {
  ["Host"]=>
  string(%s) "%s:%s"
  ["Foo-Bar"]=>
  string(3) "bar"
}
array(2) {
  ["Host"]=>
  string(%s) "%s:%s"
  ["Foo-Bar"]=>
  string(3) "bar"
}
array(2) {
  ["X-Powered-By"]=>
  string(%s) "PHP/%s"
  ["Content-Type"]=>
  string(10) "text/plain"
}
