--TEST--
Recursive mkdir() on ftp should create missing directories.
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
?>
--FILE--
<?php
$bug77680=1;

require __DIR__ . "/../../../ftp/tests/server.inc";

$path = "ftp://localhost:" . $port."/one/two/three/";
mkdir($path, 0755, true);

?>
--EXPECT--
string(20) "CWD /one/two/three
"
string(14) "CWD /one/two
"
string(10) "CWD /one
"
string(7) "CWD /
"
string(7) "MKD /
"
string(10) "MKD /one
"
string(14) "MKD /one/two
"
string(20) "MKD /one/two/three
"
