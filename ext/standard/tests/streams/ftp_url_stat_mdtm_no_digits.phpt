--TEST--
stat() on ftp:// ignores an MDTM reply without digits
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
?>
--FILE--
<?php

require __DIR__ . "/../../../ftp/tests/server.inc";

$path = "ftp://localhost:" . $port . "/mdtmnodigits";

$stat = stat($path);
var_dump($stat['size'], $stat['mtime']);
?>
--EXPECT--
int(10)
int(-1)
