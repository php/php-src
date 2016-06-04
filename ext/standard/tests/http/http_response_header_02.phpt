--TEST--
$http_reponse_header (redirect)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:22347'); ?>
--INI--
allow_url_fopen=1
allow_url_include=1
--FILE--
<?php
require 'server.inc';

$responses = array(
	"data://text/plain,HTTP/1.0 302 Found\r\n"
    . "Some: Header\r\nLocation: http://127.0.0.1:22347/try-again\r\n\r\n",
    "data://test/plain,HTTP/1.0 200 Ok\r\nSome: Header\r\n\r\nBody",
);

$pid = http_server("tcp://127.0.0.1:22347", $responses, $output);

function test() {
    $f = file_get_contents('http://127.0.0.1:22347/');
    var_dump($f);
    var_dump($http_response_header);
}
test();

http_server_kill($pid);
?>
==DONE==
--EXPECT--
string(4) "Body"
array(5) {
  [0]=>
  string(18) "HTTP/1.0 302 Found"
  [1]=>
  string(12) "Some: Header"
  [2]=>
  string(42) "Location: http://127.0.0.1:22347/try-again"
  [3]=>
  string(15) "HTTP/1.0 200 Ok"
  [4]=>
  string(12) "Some: Header"
}
==DONE==
