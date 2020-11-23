--TEST--
Bug #76342 (file_get_contents waits twice specified timeout)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

$timeout = 0.5;
$options = [
  'http' => [
    'timeout' => $timeout,
  ],
];

$ctx = stream_context_create($options);

['pid' => $pid, 'uri' => $uri] = http_server_sleep();

$start = microtime(true);
file_get_contents($uri, false, $ctx);
$diff = microtime(true) - $start;
if ($diff >= 2 * $timeout) {
    echo "FAIL: $diff\n";
}

http_server_kill($pid);

?>
DONE
--EXPECTF--
Warning: file_get_contents(http://%s:%d): Failed to open stream: HTTP request failed! in %s on line %d
DONE
