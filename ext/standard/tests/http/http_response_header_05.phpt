--TEST--
$http_reponse_header (whitespace-only "header")
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.0 200 Ok\r\n    \r\n\r\nBody",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

var_dump(http_get_last_response_headers());

$f = file_get_contents($uri);
var_dump($f);
var_dump($http_response_header);
var_dump(http_get_last_response_headers());

http_server_kill($pid);

?>
--EXPECT--
NULL
string(4) "Body"
array(2) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(0) ""
}
