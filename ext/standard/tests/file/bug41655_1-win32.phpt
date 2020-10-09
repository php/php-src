--TEST--
Bug #41655 (open_basedir bypass via glob()) 1/2
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
?>
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--INI--
open_basedir="{TMP}"
--FILE--
<?php
$a=glob("./*.jpeg");
var_dump($a);
echo "Done\n";
?>
--EXPECT--
array(0) {
}
Done
