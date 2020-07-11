--TEST--
Bug #79265 variation: "host:" not at start of header
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.0 200 OK\r\n\r\n",
);

$pid = http_server("tcp://127.0.0.1:12342", $responses, $output);

$opts = array(
  'http'=>array(
    'method'=>"GET",
    'header'=>"RandomHeader: host:8080\r\n" .
              "Cookie: foo=bar\r\n"
  )
);
$context = stream_context_create($opts);
$fd = fopen('http://127.0.0.1:12342/', 'rb', false, $context);
fseek($output, 0, SEEK_SET);
echo stream_get_contents($output);
fclose($fd);

http_server_kill($pid);

?>
--EXPECT--
GET / HTTP/1.0
Host: 127.0.0.1:12342
Connection: close
RandomHeader: host:8080
Cookie: foo=bar
