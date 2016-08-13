--TEST--
Bug #69655 (php -S changes MKCALENDAR request method to MKCOL)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start();
foreach (['MKCO', 'MKCOLL', 'M'] as $method) {
    $context = stream_context_create(['http' => ['method' => $method]]);
    // the following is supposed to emit a warning for unsupported methods
    file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS, false, $context);
}
?>
--EXPECTF--
Warning: file_get_contents(http://localhost:8964): failed to open stream: HTTP request failed! HTTP/1.0 501 Not Implemented
 in %s on line %d

Warning: file_get_contents(http://localhost:8964): failed to open stream: HTTP request failed! HTTP/1.0 501 Not Implemented
 in %s on line %d

Warning: file_get_contents(http://localhost:8964): failed to open stream: HTTP request failed! HTTP/1.0 501 Not Implemented
 in %s on line %d
