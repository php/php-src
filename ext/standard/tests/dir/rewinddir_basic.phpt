--TEST--
Test rewinddir() function : basic functionality
--FILE--
<?php
/* Prototype  : void rewinddir([resource $dir_handle])
 * Description: Rewind dir_handle back to the start
 * Source code: ext/standard/dir.c
 * Alias to functions: rewind
 */

/*
 * Test basic functionality of rewinddir()
 */

echo "*** Testing rewinddir() : basic functionality ***\n";

// include file.inc for create_files function
include(__DIR__ . "/../file/file.inc");

$dir_path1 = __DIR__ . "/rewinddir_basic_dir1";
$dir_path2 = __DIR__ . "/rewinddir_basic_dir2";
mkdir($dir_path1);
mkdir($dir_path2);

@create_files($dir_path1, 1);
@create_files($dir_path2, 1, 'numeric', 0755, 1, 'w', 'file', 2);
var_dump($dh1 = opendir($dir_path1));
var_dump($dh2 = opendir($dir_path2));

$data = array();
echo "\n-- Read and rewind first directory (argument supplied) --\n";
while(FALSE !== $file1 = readdir($dh1)) {
	$data[] = $file1;
}
$first = $data[0];
sort($data);
var_dump($data);

var_dump(rewinddir($dh1));
var_dump(readdir($dh1) == $first);

$data = array();
echo "\n-- Read and rewind second directory (no argument supplied) --\n";
while(FALSE !== $file2 = readdir()) {
	$data[] = $file2;
}
$first = $data[0];
sort($data);
var_dump($data);

var_dump(rewinddir());
var_dump(readdir() == $first);

closedir($dh1);
closedir($dh2);

delete_files($dir_path1, 1);
delete_files($dir_path2, 1, 'file', 2);
?>
===DONE===
--CLEAN--
<?php
$dir_path1 = __DIR__ . "/rewinddir_basic_dir1";
$dir_path2 = __DIR__ . "/rewinddir_basic_dir2";
rmdir($dir_path1);
rmdir($dir_path2);
?>
--EXPECTF--
*** Testing rewinddir() : basic functionality ***
resource(%d) of type (stream)
resource(%d) of type (stream)

-- Read and rewind first directory (argument supplied) --
array(3) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(9) "file1.tmp"
}
NULL
bool(true)

-- Read and rewind second directory (no argument supplied) --
array(3) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(9) "file2.tmp"
}
NULL
bool(true)
===DONE===
