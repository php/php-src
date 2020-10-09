--TEST--
Test readdir() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality of readdir()
 */

echo "*** Testing readdir() : basic functionality ***\n";

// include the file.inc for Function: function create_files()
chdir(__DIR__);
include(__DIR__."/../file/file.inc");

$path = __DIR__ . '/readdir_basic';
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
--CLEAN--
<?php
$path = __DIR__ . '/readdir_basic';
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
