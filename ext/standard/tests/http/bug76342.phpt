--TEST--
Bug #76342 (file_get_contents waits twice specified timeout)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require 'server.inc';

$options = [
  'http' => [
    'timeout' => '0.1',
  ],
];

$ctx = stream_context_create($options);

$pid = http_server_sleep('tcp://127.0.0.1:12342');

$start = microtime(true);
file_get_contents('http://127.0.0.1:12342/', false, $ctx);
if (microtime(true) - $start >= 0.2) {
    echo 'FAIL';
}

http_server_kill($pid);

?>
DONE
--EXPECTF--
Warning: file_get_contents(http://127.0.0.1:12342/): failed to open stream: HTTP request failed! in %s on line %d
DONE
