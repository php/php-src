--TEST--
Bug #41518 (file_exists() warns of open_basedir restriction on non-existent file)
--SKIPIF--
<?php
$tmp_dir = __DIR__ . '/bug41518';
mkdir($tmp_dir);
if (!is_dir($tmp_dir)) {
    die("skip");
}
@unlink($tmp_dir);
?>
--INI--
open_basedir=.
--FILE--
<?php

$tmp_dir = __DIR__ . "/bug41518/";
@mkdir($tmp_dir);
$tmp_file = $tmp_dir."/bug41418.tmp";

touch($tmp_file);
var_dump(file_exists($tmp_file)); //exists
var_dump(file_exists($tmp_file."nosuchfile")); //doesn't exist

@unlink($tmp_file);
@rmdir($tmp_dir);
echo "Done\n";
?>
--CLEAN--
<?php
$tmp_dir = __DIR__ . "/bug41518/";
@unlink($tmp_dir);
?>
--EXPECT--
bool(true)
bool(false)
Done
