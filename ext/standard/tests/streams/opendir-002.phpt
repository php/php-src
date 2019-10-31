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

$path="ftp://localhost:" . $port."/";

$ds=opendir($path);
var_dump($ds);

while ($fn=readdir($ds)) {
      var_dump($fn);
}

closedir($ds);
?>
--EXPECTF--
resource(%d) of type (stream)
string(5) "file1"
string(5) "file1"
string(3) "fil"
string(4) "b0rk"
