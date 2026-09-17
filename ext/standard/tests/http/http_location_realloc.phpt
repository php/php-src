--TEST--
HTTP response with two Location headers (second longer) triggers erealloc
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.1 301 Moved Permanently\r\nLocation: /short\r\nLocation: /a_much_longer_path_than_short\r\nContent-Length: 0\r\n\r\n",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$ctx = stream_context_create(['http' => ['follow_location' => 0]]);
$result = file_get_contents("$uri/", false, $ctx);
var_dump($result);
var_dump(http_get_last_response_headers());

http_server_kill($pid);
?>
--EXPECT--
string(0) ""
array(4) {
  [0]=>
  string(30) "HTTP/1.1 301 Moved Permanently"
  [1]=>
  string(16) "Location: /short"
  [2]=>
  string(40) "Location: /a_much_longer_path_than_short"
  [3]=>
  string(17) "Content-Length: 0"
}
