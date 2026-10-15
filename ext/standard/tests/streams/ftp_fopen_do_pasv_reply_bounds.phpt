--TEST--
ftp:// wrapper rejects a PASV reply whose address ends at the reply buffer limit
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
?>
--FILE--
<?php

$pasv_reply_at_buffer_end = true;
require __DIR__ . "/../../../ftp/tests/server.inc";

$path = "ftp://localhost:" . $port . "/pasvbufferend";

var_dump(fopen($path, "r"));
?>
--EXPECTF--
Warning: fopen(%s): Failed to open stream: FTP server reports %a in %s on line %d
bool(false)
