--TEST--
Test readdir() function : basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : string readdir([resource $dir_handle])
 * Description: Read directory entry from dir_handle
 * Source code: ext/standard/dir.C
 */

/*
 * Test basic functionality of readdir()
 */

echo "*** Testing readdir() : basic functionality ***\n";

// include the file.inc for Function: function create_files()
chdir(__DIR__);
include(__DIR__."/../file/file.inc");

$path = __DIR__ . '/私はガラスを食べられますreaddir_basic';
mkdir($path);
create_files($path, 3);

echo "\n-- Call readdir() with \$path argument --\n";
var_dump($dh = opendir($path));
$a = array();
while( FALSE !== ($file = readdir($dh)) ) {
	$a[] = $file;
}
sort($a);
foreach($a as $file) {
	var_dump($file);
}

echo "\n-- Call readdir() without \$path argument --\n";
var_dump($dh = opendir($path));
$a = array();
while( FALSE !== ( $file = readdir() ) ) {
	$a[] = $file;
}
sort($a);
foreach($a as $file) {
	var_dump($file);
}

delete_files($path, 3);
closedir($dh);
?>
===DONE===
--CLEAN--
<?php
$path = __DIR__ . '/私はガラスを食べられますreaddir_basic';
rmdir($path);
?>
--EXPECTF--
*** Testing readdir() : basic functionality ***

-- Call readdir() with $path argument --
resource(%d) of type (stream)
string(1) "."
string(2) ".."
string(9) "file1.tmp"
string(9) "file2.tmp"
string(9) "file3.tmp"

-- Call readdir() without $path argument --
resource(%d) of type (stream)
string(1) "."
string(2) ".."
string(9) "file1.tmp"
string(9) "file2.tmp"
string(9) "file3.tmp"
===DONE===
