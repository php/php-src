--TEST--
Bug #41518 (file_exists() warns of open_basedir restriction on non-existent file)
--SKIPIF--
<?php
/* let's use /tmp here */
$tmp_dir = __DIR__ . '/tmp';
mkdir($tmp_dir);
if (!is_dir($tmp_dir)) {
	die("skip");
}
?>
--INI--
open_basedir=.
--FILE--
<?php

$tmp_dir = "/tmp";
$tmp_file = $tmp_dir."/bug41418.tmp";

touch($tmp_file);
var_dump(file_exists($tmp_file)); //exists
var_dump(file_exists($tmp_file."nosuchfile")); //doesn't exist

@unlink($tmp_file);
@rmdir($tmp_dir);
echo "Done\n";
?>
--EXPECT--
bool(true)
bool(false)
Done
