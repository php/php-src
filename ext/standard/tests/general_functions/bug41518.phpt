--TEST--
Bug #41518 (file_exists() warns of open_basedir restriction on non-existent file)
--SKIPIF--
<?php
/* let's use /tmp here */
$tmp_dir = "/tmp";
if (!is_dir($tmp_dir) || realpath($tmp_dir) !== $tmp_dir) {
	die("skip");
}
?>
--INI--
open_basedir=/tmp/
--FILE--
<?php

$tmp_dir = "/tmp";
$tmp_file = $tmp_dir."/bug41418.tmp";

touch($tmp_file);
var_dump(file_exists($tmp_file)); //exists
var_dump(file_exists($tmp_file."nosuchfile")); //doesn't exist

@unlink($tmp_file);

echo "Done\n";
?>
--EXPECTF--	
bool(true)
bool(false)
Done
