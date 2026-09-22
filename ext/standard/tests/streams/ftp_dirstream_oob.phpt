--TEST--
opendir() with 'ftp://' stream must not go out of bounds on an empty or slash-only listing line
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
?>
--FILE--
<?php

/* An empty line makes php_basename() return "\n" and a slash-only final line
 * makes it return "", the two lengths the buffer math underflowed on. */
$nlst_data = "file1\r\n\nb0rk\r\n/";

require __DIR__ . "/../../../ftp/tests/server.inc";

$path="ftp://localhost:" . $port."/";

$ds=opendir($path);
var_dump($ds);

while (($fn=readdir($ds)) !== false) {
      var_dump($fn);
}

closedir($ds);
?>
--EXPECTF--
resource(%d) of type (stream)
string(5) "file1"
string(0) ""
string(4) "b0rk"
string(0) ""
