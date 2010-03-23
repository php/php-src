--TEST--
Test dir() function : basic functionality
--FILE--
<?php
/* 
 * Prototype  : object dir(string $directory[, resource $context])
 * Description: Directory class with properties, handle and class and methods read, rewind and close
 * Source code: ext/standard/dir.c
 */

echo "*** Testing dir() : basic functionality ***\n";

// include the file.inc for Function: function create_files()
include(dirname(__FILE__)."/../file/file.inc");

// create the temporary directory
$file_path = dirname(__FILE__);
$dir_path = $file_path."/dir_basic";
@mkdir($dir_path);

// create files within the temporary directory
create_files($dir_path, 3, "alphanumeric", 0755, 1, "w", "dir_basic");

echo "Get Directory instance:\n";
$d = dir($dir_path);
var_dump( $d );

echo "\nRead and rewind:\n";
var_dump( $d->read() );
var_dump( $d->read() );
var_dump( $d->rewind() );

echo "\nTest using handle directly:\n";
var_dump( readdir($d->handle) );
var_dump( readdir($d->handle) );

echo "\nClose directory:\n";
var_dump( $d->close() );
var_dump( $d );

echo "\nTest read after closing the dir:";
var_dump( $d->read() );

// delete temp files
delete_files($dir_path, 3, "dir_basic", 1, ".tmp");
echo "Done";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$dir_path = $file_path."/dir_basic";

rmdir($dir_path);
?>
--EXPECTF--
*** Testing dir() : basic functionality ***
Get Directory instance:
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/dir_basic"
  ["handle"]=>
  resource(%d) of type (stream)
}

Read and rewind:
string(%d) "%s"
string(%d) "%s"
NULL

Test using handle directly:
string(%d) "%s"
string(%d) "%s"

Close directory:
NULL
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/dir_basic"
  ["handle"]=>
  resource(%d) of type (Unknown)
}

Test read after closing the dir:
Warning: Directory::read(): %d is not a valid Directory resource in %s on line %d
bool(false)
Done