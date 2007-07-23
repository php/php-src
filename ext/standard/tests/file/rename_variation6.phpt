--TEST--
Test rename() function: usage variations-7
--SKIPIF--
<?php
if (!function_exists("symlink")) die("skip symlinks are not supported");
?>
--FILE--
<?php

$tmp_file = __FILE__.".tmp";
$tmp_link = __FILE__.".tmp.link";
$tmp_link2 = __FILE__.".tmp.link2";

touch($tmp_file);
symlink($tmp_file, $tmp_link);
rename($tmp_link, $tmp_link2);

clearstatcache();

var_dump(readlink($tmp_link));
var_dump(readlink($tmp_link2));
var_dump(file_exists($tmp_file));

@unlink($tmp_link);
@unlink($tmp_link2);
@unlink($tmp_file);

echo "Done\n";
?>
--EXPECTF--	
Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%srename_variation6.php.tmp"
bool(true)
Done
