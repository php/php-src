--TEST--
$http_reponse_header (no redirect)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.0 200 Ok\r\nSome: Header\r\nSome: Header\r\n\r\nBody",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$f = file_get_contents($uri);
var_dump($f);
var_dump($http_response_header);

http_server_kill($pid);

--EXPECT--
string(4) "Body"
array(3) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(12) "Some: Header"
  [2]=>
  string(12) "Some: Header"
}
