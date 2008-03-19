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
while( FALSE !== ($file = readdir($dh)) ) {
	var_dump($file);
}

echo "\ncall fseek() on directory resource:\n";
var_dump(fseek($dh, 20));

echo "call readdir():\n";
while( FALSE !== ($file = readdir($dh)) ) {
	var_dump($file);
}

echo "\ncall fseek() with different arguments on directory resource:\n";
var_dump(fseek($dh, 20, SEEK_END));

echo "call readdir():\n";
while( FALSE !== ($file = readdir($dh)) ) {
	var_dump($file);
}

delete_files($path, 3);
closedir($dh);
var_dump(rmdir($path));

?>
--EXPECTF--
call readdir():
resource(%d) of type (stream)
string(1) "."
string(2) ".."
string(9) "file1.tmp"
string(9) "file2.tmp"
string(9) "file3.tmp"

call fseek() on directory resource:
int(0)
call readdir():
string(1) "."
string(2) ".."
string(9) "file1.tmp"
string(9) "file2.tmp"
string(9) "file3.tmp"

call fseek() with different arguments on directory resource:
int(0)
call readdir():
string(1) "."
string(2) ".."
string(9) "file1.tmp"
string(9) "file2.tmp"
string(9) "file3.tmp"
bool(true)
--UEXPECTF--
call readdir():
resource(%d) of type (stream)
unicode(1) "."
unicode(2) ".."
unicode(9) "file1.tmp"
unicode(9) "file2.tmp"
unicode(9) "file3.tmp"

call fseek() on directory resource:
int(0)
call readdir():
unicode(1) "."
unicode(2) ".."
unicode(9) "file1.tmp"
unicode(9) "file2.tmp"
unicode(9) "file3.tmp"

call fseek() with different arguments on directory resource:
int(0)
call readdir():
unicode(1) "."
unicode(2) ".."
unicode(9) "file1.tmp"
unicode(9) "file2.tmp"
unicode(9) "file3.tmp"
bool(true)
