--TEST--
Bug #65538: SSL context "cafile" disallows URL stream wrappers
--SKIPIF--
<?php
if (!extension_loaded('openssl')) die('skip, openssl required');
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
--FILE--
<?php
$clientCtx = stream_context_create(['ssl' => [
    'cafile' => 'http://curl.haxx.se/ca/cacert.pem'
]]);
file_get_contents('https://github.com', false, $clientCtx);
--EXPECTF--
Warning: remote cafile streams are disabled for security purposes in %s on line %d

Warning: file_get_contents(): Failed to enable crypto in %s on line %d

Warning: file_get_contents(%s): failed to open stream: operation failed in %s on line %d
