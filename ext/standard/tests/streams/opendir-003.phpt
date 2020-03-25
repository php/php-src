--TEST--
opendir() with 'ftps://' stream.
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
if (!extension_loaded('openssl')) die ("skip openssl not available.");
?>
--FILE--
<?php

$ssl=true;
require __DIR__ . "/../../../ftp/tests/server.inc";

$path="ftps://127.0.0.1:" . $port."/bogusdir";

$context = stream_context_create(array('ssl' => array('cafile' =>  __DIR__ . '/../../../ftp/tests/cert.pem')));

var_dump(opendir($path, $context));
?>
--EXPECTF--
Warning: opendir(ftps://127.0.0.1:%d/bogusdir): Failed to open directory: FTP server reports 250 /bogusdir: No such file or directory
 in %s on line %d
bool(false)
