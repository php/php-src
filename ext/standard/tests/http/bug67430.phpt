--TEST--
Bug #67430 (http:// wrapper doesn't follow 308 redirects)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

function do_test($follow) {
  $options = [
    'http' => [
      'method' => 'POST',
      'follow_location' => $follow,
    ],
  ];

  $ctx = stream_context_create($options);

  $responses = [
    "data://text/plain,HTTP/1.1 308\r\nLocation: /foo\r\n\r\n",
    "data://text/plain,HTTP/1.1 200\r\nConnection: close\r\n\r\n",
  ];
  ['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

  $fd = fopen($uri, 'rb', false, $ctx);
  fseek($output, 0, SEEK_SET);
  echo stream_get_contents($output);

  http_server_kill($pid);
}

do_test(true);
do_test(false);

?>
Done
--EXPECTF--
POST / HTTP/1.1
Host: %s:%d
Connection: close

GET /foo HTTP/1.1
Host: %s:%d
Connection: close

POST / HTTP/1.1
Host: %s:%d
Connection: close

Done
