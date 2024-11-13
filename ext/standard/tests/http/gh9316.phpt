--TEST--
Bug GH-9316 ($http_response_header is wrong for long status line)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.1 200 Some very long reason-phrase to test that this is properly handled by our code without adding a new header like  Bad: Header\r\nGood: Header\r\n\r\nBody",
    "data://text/plain,HTTP/1.1 200 \r\nGood: Header\r\n\r\nBody",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

for ($i = 0; $i < count($responses); ++$i) {
    echo 'http_get_last_response_headers() before stream layer call:', PHP_EOL;
    var_dump(http_get_last_response_headers());

    $f = @fopen($uri, "r");
    echo '$http_response_header', PHP_EOL;
    var_dump($http_response_header);
    echo 'http_get_last_response_headers() after stream layer call:', PHP_EOL;
    var_dump(http_get_last_response_headers());
    fclose($f);
}

http_server_kill($pid);

?>
--EXPECT--
http_get_last_response_headers() before stream layer call:
NULL
$http_response_header
array(2) {
  [0]=>
  string(126) "HTTP/1.1 200 Some very long reason-phrase to test that this is properly handled by our code without adding a new header like  "
  [1]=>
  string(12) "Good: Header"
}
http_get_last_response_headers() after stream layer call:
array(2) {
  [0]=>
  string(126) "HTTP/1.1 200 Some very long reason-phrase to test that this is properly handled by our code without adding a new header like  "
  [1]=>
  string(12) "Good: Header"
}
http_get_last_response_headers() before stream layer call:
array(2) {
  [0]=>
  string(126) "HTTP/1.1 200 Some very long reason-phrase to test that this is properly handled by our code without adding a new header like  "
  [1]=>
  string(12) "Good: Header"
}
$http_response_header
array(2) {
  [0]=>
  string(13) "HTTP/1.1 200 "
  [1]=>
  string(12) "Good: Header"
}
http_get_last_response_headers() after stream layer call:
array(2) {
  [0]=>
  string(13) "HTTP/1.1 200 "
  [1]=>
  string(12) "Good: Header"
}
