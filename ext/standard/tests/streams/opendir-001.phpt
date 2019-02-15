--TEST--
opendir() with 'ftp://' stream.
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
?>
--FILE--
<?php

require __DIR__ . "/../../../ftp/tests/server.inc";

$path="ftp://localhost:" . $port."/bogusdir";

var_dump(opendir($path));
?>
==DONE==
--EXPECTF--
Warning: opendir(ftp://localhost:%d/bogusdir): failed to open dir: FTP server reports 250 /bogusdir: No such file or directory
 in %s on line %d
bool(false)
==DONE==
