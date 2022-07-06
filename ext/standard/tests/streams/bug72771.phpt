--TEST--
Bug #72771. FTPS to FTP downgrade not allowed when server doesn't support AUTH TLS or AUTH SSL.
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
if (!extension_loaded('openssl')) die ("skip openssl not available.");
?>
--FILE--
<?php

require __DIR__ . "/../../../ftp/tests/server.inc";

$path="ftps://127.0.0.1:" . $port."/";

$ds=opendir($path, $context);
var_dump($ds);
?>
--EXPECTF--
Warning: opendir(ftps://127.0.0.1:%d/): Failed to open directory: Server doesn't support FTPS. in %s on line %d
bool(false)
