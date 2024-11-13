--TEST--
Verify that http_clear_last_response_headers() clears the headers.
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
var_dump(http_get_last_response_headers());

// Clear headers
http_clear_last_response_headers();
var_dump(http_get_last_response_headers());

http_server_kill($pid);

?>
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
NULL
