--TEST--
Test rename() function: usage variations-8
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip..  not for Windows');
if (!function_exists("symlink")) die("skip symlinks are not supported");
?>
--FILE--
<?php

$tmp_link = __FILE__.".tmp.link";
$tmp_link2 = __FILE__.".tmp.link2";

symlink(dirname(__FILE__)."/there_is_no_such_file", $tmp_link);
rename($tmp_link, $tmp_link2);

clearstatcache();

var_dump(readlink($tmp_link));
var_dump(readlink($tmp_link2));

@unlink($tmp_link);
@unlink($tmp_link2);

echo "Done\n";
?>
--EXPECTF--
Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%sthere_is_no_such_file"
Done
