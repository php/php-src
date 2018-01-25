--TEST--
Test dir() function : usage variations - operate on previously opened directory
--FILE--
<?php
/*
 * Prototype  : object dir(string $directory[, resource $context])
 * Description: Directory class with properties, handle and class and methods read, rewind and close
 * Source code: ext/standard/dir.c
 */

/*
 * Testing the behavior of dir() function by trying to open a
 * directory which is already open.
 */

echo "*** Testing dir() : operate on previously opened directory ***\n";

// include the file.inc for Function: function create_files()
include( dirname(__FILE__)."/../file/file.inc");

// create the temporary directory
$file_path = dirname(__FILE__);
$dir_path = $file_path."/dir_variation4";
@mkdir($dir_path);

// create files within the temporary directory
create_files($dir_path, 3, "alphanumeric", 0755, 1, "w", "dir_variation4");

// open the directory
$d = dir($dir_path);
var_dump( $d );

// open the same directory again without closing it
$e = dir($dir_path);
var_dump( $e );

echo "-- reading directory contents with previous handle --\n";
var_dump( $d->read() ); // with previous handle

echo "-- reading directory contents with current handle --\n";
var_dump( $e->read() ); // with current handle

// delete temporary files
delete_files($dir_path, 3, "dir_variation4");
echo "Done";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$dir_path = $file_path."/dir_variation4";

rmdir($dir_path);
?>
--EXPECTF--
*** Testing dir() : operate on previously opened directory ***
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/dir_variation4"
  ["handle"]=>
  resource(%d) of type (stream)
}
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/dir_variation4"
  ["handle"]=>
  resource(%d) of type (stream)
}
-- reading directory contents with previous handle --
string(%d) "%s"
-- reading directory contents with current handle --
string(%d) "%s"
Done
