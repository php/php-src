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
    $f = @fopen($uri, "r");
    var_dump($http_response_header);
    fclose($f);
}

http_server_kill($pid);

?>
--EXPECT--
array(2) {
  [0]=>
  string(126) "HTTP/1.1 200 Some very long reason-phrase to test that this is properly handled by our code without adding a new header like  "
  [1]=>
  string(12) "Good: Header"
}
array(2) {
  [0]=>
  string(13) "HTTP/1.1 200 "
  [1]=>
  string(12) "Good: Header"
}
