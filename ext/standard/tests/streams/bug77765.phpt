--TEST--
stat() on directory should return 40755 for ftp://
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
?>
--FILE--
<?php

require __DIR__ . "/../../../ftp/tests/server.inc";

$path = "ftp://localhost:" . $port."/www";

var_dump(stat($path)['mode']);
?>
--EXPECT--
string(11) "SIZE /www
"
int(16877)
