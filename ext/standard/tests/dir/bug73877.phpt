--TEST--
Bug #73877 readlink() returns garbage for UTF-8 paths
File type functions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
?>
--FILE--
<?php

$base = dirname(__FILE__) . DIRECTORY_SEPARATOR . "bug73877";
$dir0 = $base . DIRECTORY_SEPARATOR . "bug73877";
$dir1 = $base . DIRECTORY_SEPARATOR . "Серёжка";
$junk0 = $base . DIRECTORY_SEPARATOR . "Серёжка2";

mkdir($base);
mkdir($dir0);
mkdir($dir1);
`mklink /J $junk0 $dir0`;

var_dump(
	readlink($dir0),
	readlink($dir1),
	readlink($junk0),
	strlen(readlink($dir0)) === strlen(readlink($junk0))
);

?>
--CLEAN--
<?php

$base = dirname(__FILE__) . DIRECTORY_SEPARATOR . "bug73877";
$dir0 = $base . DIRECTORY_SEPARATOR . "bug73877";
$dir1 = $base . DIRECTORY_SEPARATOR . "Серёжка";
$junk0 = $base . DIRECTORY_SEPARATOR . "Серёжка2";

rmdir($junk0);
rmdir($dir0);
rmdir($dir1);
rmdir($base);

?>
--EXPECTF--
string(%d) "%sbug73877"
string(%d) "%sСерёжка"
string(%d) "%sbug73877"
bool(true)
