--TEST--
Testing fseek() on a directory stream
--FILE--
<?php

// include the file.inc for Function: function create_files()
require(dirname(__FILE__) . '/file.inc');

$path = dirname(__FILE__) . '/fseek_dir_basic';
mkdir($path);
create_files($path, 3);

echo "call readdir():\n";
var_dump($dh = opendir($path));
$files = array();
while( FALSE !== ($files[] = readdir($dh)) ) {}
sort($files);
var_dump($files);
$files = array();

echo "\ncall fseek() on directory resource:\n";
var_dump(fseek($dh, 20));

echo "call readdir():\n";
while( FALSE !== ($files[] = readdir($dh)) ) {}
sort($files);
var_dump($files);
$files = array();

echo "\ncall fseek() with different arguments on directory resource:\n";
var_dump(fseek($dh, 20, SEEK_END));

echo "call readdir():\n";
while( FALSE !== ($files[] = readdir($dh)) ) {}
sort($files);
var_dump($files);

delete_files($path, 3);
closedir($dh);
var_dump(rmdir($path));

?>
--EXPECTF--
call readdir():
resource(%d) of type (stream)
array(6) {
  [0]=>
  bool(false)
  [1]=>
  string(1) "."
  [2]=>
  string(2) ".."
  [3]=>
  string(9) "file1.tmp"
  [4]=>
  string(9) "file2.tmp"
  [5]=>
  string(9) "file3.tmp"
}

call fseek() on directory resource:
int(0)
call readdir():
array(6) {
  [0]=>
  bool(false)
  [1]=>
  string(1) "."
  [2]=>
  string(2) ".."
  [3]=>
  string(9) "file1.tmp"
  [4]=>
  string(9) "file2.tmp"
  [5]=>
  string(9) "file3.tmp"
}

call fseek() with different arguments on directory resource:
int(0)
call readdir():
array(6) {
  [0]=>
  bool(false)
  [1]=>
  string(1) "."
  [2]=>
  string(2) ".."
  [3]=>
  string(9) "file1.tmp"
  [4]=>
  string(9) "file2.tmp"
  [5]=>
  string(9) "file3.tmp"
}
bool(true)
