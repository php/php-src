--TEST--
Bug #75535: Inappropriately parsing HTTP response leads to PHP segment fault
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:22351'); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = array(
	"data://text/plain,HTTP/1.0 200 Ok\r\nContent-Length\r\n",
);

$pid = http_server("tcp://127.0.0.1:22351", $responses, $output);

var_dump(file_get_contents('http://127.0.0.1:22351/'));
var_dump($http_response_header);

http_server_kill($pid);
?>
==DONE==
--EXPECT--
string(0) ""
array(2) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(14) "Content-Length"
}
==DONE==
