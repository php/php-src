--TEST--
Test rename() function: usage variations-8
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip.. for Windows');
if (!function_exists("symlink")) die("skip symlinks are not supported");
?>
--FILE--
<?php

$tmp_link = __FILE__.".tmp.link";
$tmp_link2 = __FILE__.".tmp.link2";

if (symlink(dirname(__FILE__)."/there_is_no_such_file", $tmp_link)) {
	rename($tmp_link, $tmp_link2);
}

clearstatcache();

var_dump(readlink($tmp_link));
var_dump(readlink($tmp_link2));

@unlink($tmp_link);
@unlink($tmp_link2);

echo "Done\n";
?>
--EXPECTF--	
Warning: symlink(): Could not fetch file information(error 2) in %srename_variation7-win32.php on line %d

Warning: readlink(): readlink failed to read the symbolic link (%srename_variation7-win32.php.tmp.link), error 2) in %srename_variation7-win32.php on line %d
bool(false)

Warning: readlink(): readlink failed to read the symbolic link (%srename_variation7-win32.php.tmp.link2), error 2) in %srename_variation7-win32.php on line %d
bool(false)
Done
