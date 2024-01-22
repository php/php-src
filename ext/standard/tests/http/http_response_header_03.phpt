--TEST--
$http_reponse_header (redirect + not found)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.0 302 Found\r\n"
    . "Some: Header\r\nLocation: /try-again\r\n\r\n",
    "data://test/plain,HTTP/1.0 404 Not Found\r\nSome: Header\r\n\r\nBody",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$f = file_get_contents($uri);
var_dump($f);
var_dump($http_response_header);

http_server_kill($pid);

?>
--EXPECTF--
Warning: file_get_contents(http://%s:%d): Failed to open stream: HTTP request failed! HTTP/1.0 404 Not Found%a
bool(false)
array(5) {
  [0]=>
  string(18) "HTTP/1.0 302 Found"
  [1]=>
  string(12) "Some: Header"
  [2]=>
  string(20) "Location: /try-again"
  [3]=>
  string(22) "HTTP/1.0 404 Not Found"
  [4]=>
  string(12) "Some: Header"
}
