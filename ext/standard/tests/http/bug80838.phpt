--TEST--
Bug #80838 (HTTP wrapper waits for HTTP 1 response after HTTP 101)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require 'server.inc';

$responses = [
  "data://text/plain,HTTP/1.1 101 Switching Protocols\r\nHeader1: Value1\r\nHeader2: Value2\r\n\r\n"
    . "Hello from another protocol"
];

$pid = http_server('tcp://127.0.0.1:12342', $responses);

$options = [
  'http' => [
    'ignore_errors' => true
  ],
];

$ctx = stream_context_create($options);

$fd = fopen('http://127.0.0.1:12342/', 'rb', false, $ctx);
fclose($fd);
var_dump($http_response_header);

http_server_kill($pid);

?>
--EXPECT--
array(3) {
  [0]=>
  string(32) "HTTP/1.1 101 Switching Protocols"
  [1]=>
  string(15) "Header1: Value1"
  [2]=>
  string(15) "Header2: Value2"
}
