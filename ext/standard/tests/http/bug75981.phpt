--TEST--
Bug #75981 (stack-buffer-overflow while parsing HTTP response)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
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
    "data://text/plain,000000000100\xA\xA"
];
['pid' => $pid, 'uri' => $uri] = http_server($responses);

echo @file_get_contents($uri, false, $ctx);

http_server_kill($pid);

?>
DONE
--EXPECT--
DONE
