--TEST--
Test readdir() function : usage variations - empty directories
--FILE--
<?php
/* Prototype  : string readdir([resource $dir_handle])
 * Description: Read directory entry from dir_handle
 * Source code: ext/standard/dir.c
 */

/*
 * Pass readdir() a directory handle pointing to an empty directory to test behaviour
 */

echo "*** Testing readdir() : usage variations ***\n";

$path = dirname(__FILE__) . '/readdir_variation2';
mkdir($path);
$dir_handle = opendir($path);

echo "\n-- Pass an empty directory to readdir() --\n";
function mysort($a,$b) {
	return strlen($a) > strlen($b) ? 1 : -1;
}
$entries = array();
while(FALSE !== ($file = readdir($dir_handle))){
	$entries[] = $file;
}

closedir($dir_handle);

usort($entries, "mysort");
foreach($entries as $entry) {
	var_dump($entry);
}
?>
===DONE===
--CLEAN--
<?php
$path = dirname(__FILE__) . '/readdir_variation2';
rmdir($path);
?>
--EXPECTF--
*** Testing readdir() : usage variations ***

-- Pass an empty directory to readdir() --
string(1) "."
string(2) ".."
===DONE===
