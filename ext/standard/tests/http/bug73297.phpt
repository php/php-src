--TEST--
Bug #73297 (Ignore 100 Continue returned by HTTP/1.1 servers)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require 'server.inc';

$options = [
  'http' => [
    'protocol_version' => '1.1',
    'header' => 'Connection: Close'
  ],
];

$ctx = stream_context_create($options);

$responses = [
  "data://text/plain,HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\n\r\n"
    . "Hello"
];
$pid = http_server('tcp://127.0.0.1:12342', $responses);

echo file_get_contents('http://127.0.0.1:12342/', false, $ctx);
echo "\n";

http_server_kill($pid);

?>
--EXPECT--
Hello
