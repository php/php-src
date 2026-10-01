--TEST--
http.auto_decode stream context option controls chunked response decoding
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$chunked = "data://text/plain,HTTP/1.1 200 OK\r\n"
         . "Transfer-Encoding: chunked\r\n\r\n"
         . "2\r\nab\r\n2\r\ncd\r\n0\r\n\r\n";

$responses = [$chunked, $chunked, $chunked];
['pid' => $pid, 'uri' => $uri] = http_server($responses);

function test_auto_decode($auto_decode) {
    global $uri;
    $ctx = null;
    if ($auto_decode !== null) {
        $ctx = stream_context_create(['http' => ['auto_decode' => $auto_decode]]);
    }
    $body = file_get_contents($uri, false, $ctx);

    $has_te = false;
    foreach (http_get_last_response_headers() as $h) {
        if (stripos($h, 'Transfer-Encoding:') === 0) {
            $has_te = true;
            break;
        }
    }

    return [addcslashes($body, "\r\n"), $has_te];
}

var_dump(test_auto_decode(null));
var_dump(test_auto_decode(true));
var_dump(test_auto_decode(false));

http_server_kill($pid);
?>
--EXPECT--
array(2) {
  [0]=>
  string(4) "abcd"
  [1]=>
  bool(false)
}
array(2) {
  [0]=>
  string(4) "abcd"
  [1]=>
  bool(false)
}
array(2) {
  [0]=>
  string(31) "2\r\nab\r\n2\r\ncd\r\n0\r\n\r\n"
  [1]=>
  bool(true)
}
