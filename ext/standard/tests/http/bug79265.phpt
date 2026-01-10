--TEST--
Bug #79265 (Improper injection of Host header when using fopen for http requests)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

$responses = array(
    "data://text/plain,HTTP/1.1 200 OK\r\n\r\n",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$opts = array(
  'http'=>array(
    'method'=>"GET",
    'header'=>"RandomHeader: localhost:8080\r\n" .
              "Cookie: foo=bar\r\n" .
              "Host: userspecifiedvalue\r\n"
  )
);
$context = stream_context_create($opts);
$fd = fopen($uri, 'rb', false, $context);
fseek($output, 0, SEEK_SET);
echo stream_get_contents($output);
fclose($fd);

http_server_kill($pid);

?>
--EXPECT--
GET / HTTP/1.1
Connection: close
RandomHeader: localhost:8080
Cookie: foo=bar
Host: userspecifiedvalue
