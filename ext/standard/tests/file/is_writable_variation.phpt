--TEST--
Test is_writable() and its alias is_writeable() function: usage variations
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for LINUX');
}
// Skip if being run by root (files are always readable, writeable and executable)
$filename = dirname(__FILE__)."/is_readable_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
	unlink ($filename);
	die('skip...cannot be run as root\n');
}

unlink($filename);
?>
--FILE--
<?php
/* Prototype: bool is_writable ( string $filename );
   Description: Tells whether the filename is writable.

   is_writeable() is an alias of is_writable()
*/

require dirname(__FILE__).'/file.inc';
echo "*** Testing is_writable(): usage variations ***\n";

$file_path = dirname(__FILE__);
mkdir("$file_path/is_writable");

//create a temporary file
$fp = fopen("$file_path/is_writable/bar.tmp", "w");
fclose($fp);

/* array of files to be tested to check if they are writable
   using is_writable() function */
$files_arr = array(
  "$file_path/is_writable/bar.tmp",

  /* Testing a file trailing slash */
  "$file_path/is_writable/bar.tmp/",

  /* Testing file with double slashes */
  "$file_path/is_writable//bar.tmp",
  "$file_path//is_writable//bar.tmp",
  "$file_path/is_writable/*.tmp",
  "$file_path/is_writable/b*.tmp", 

  /* Testing Binary safe */
  "$file_path/is_writable".chr(0)."bar.tmp",
  "$file_path".chr(0)."is_writable/bar.tmp",
  b"$file_path/is_writable/bar.tmp",
  
  /* Testing directories */
  ".",  // current directory, exp: bool(true)
  "$file_path/is_writable"  // temp directory, exp: bool(true)
);
$counter = 1;
/* loop through to test each element in the above array 
   is a writable file */
foreach($files_arr as $file) {
  echo "-- Iteration $counter --\n";
  var_dump( is_writable($file) );
  var_dump( is_writeable($file) );
  $counter++;
  clearstatcache();
}

echo "\n*** Testing is_writable() on directory without write permission ***\n";
chmod("$file_path/is_writable", 0004);
var_dump( is_writable("$file_path/is_writable") );  // exp: bool(false)
var_dump( is_writeable("$file_path/is_writable") );  // exp: bool(false)
chmod("$file_path/is_writable", 0777);  // chmod to enable deletion of directory

echo "\n*** Testing miscelleneous input for is_writable() function ***\n";
$name_prefix = "is_writable";
create_files(dirname(__FILE__), 1, "numeric", 0755, 1, "w", $name_prefix, 1);
create_files(dirname(__FILE__), 1, "text", 0755, 1, "w", $name_prefix, 2);
create_files(dirname(__FILE__), 1, "empty", 0755, 1, "w", $name_prefix, 3);
create_files(dirname(__FILE__), 1, "numeric", 0555, 1, "w", $name_prefix, 4);
create_files(dirname(__FILE__), 1, "text", 0222, 1, "w", $name_prefix, 5);
create_files(dirname(__FILE__), 1, "numeric", 0711, 1, "w", $name_prefix, 6);
create_files(dirname(__FILE__), 1, "text", 0114, 1, "w", $name_prefix, 7);
create_files(dirname(__FILE__), 1, "numeric", 0244, 1, "w", $name_prefix, 8);
create_files(dirname(__FILE__), 1, "text", 0421, 1, "w", $name_prefix, 9);
create_files(dirname(__FILE__), 1, "text", 0422, 1, "w", $name_prefix, 10);

$misc_files = array (
  "$file_path/is_writable1.tmp",
  "$file_path/is_writable2.tmp",
  "$file_path/is_writable3.tmp",
  "$file_path/is_writable4.tmp",
  "$file_path/is_writable5.tmp",
  "$file_path/is_writable6.tmp",
  "$file_path/is_writable7.tmp",
  "$file_path/is_writable8.tmp",
  "$file_path/is_writable9.tmp",
  "$file_path/is_writable10.tmp"
);

$counter = 1;
/* loop through to test each element in the above array
   is a writable file */
foreach($misc_files as $misc_file) {
  echo "-- Iteration $counter --\n";
  var_dump( is_writable($misc_file) );
  var_dump( is_writeable($misc_file) );
  $counter++;
  clearstatcache();
}

// change all file's permissions to 777 before deleting
change_file_perms($file_path, 10, 0777, $name_prefix);
delete_files($file_path, 10, $name_prefix);

$file_handle = fopen(__FILE__, "r");
unset($file_handle);

clearstatcache();
echo "\n*** Testing is_writable() on miscelleneous filenames ***\n";
$misc_files = array(
  0,
  1234,
  -2.34555,
  "string",
  TRUE,
  FALSE,
  NULL,
  @array(),
  @$file_handle,
  new stdclass
);
/* loop through to test each element in the above array 
   is a writable file */
foreach( $misc_files as $misc_file ) {
  var_dump( is_writable($misc_file) );
  var_dump( is_writeable($misc_file) );
  clearstatcache();
}

echo "Done\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/is_writable/bar.tmp");
rmdir(dirname(__FILE__)."/is_writable/");
?>
--EXPECTF--
*** Testing is_writable(): usage variations ***
-- Iteration 1 --
bool(true)
bool(true)
-- Iteration 2 --
bool(false)
bool(false)
-- Iteration 3 --
bool(true)
bool(true)
-- Iteration 4 --
bool(true)
bool(true)
-- Iteration 5 --
bool(false)
bool(false)
-- Iteration 6 --
bool(false)
bool(false)
-- Iteration 7 --
bool(true)
bool(true)
-- Iteration 8 --
bool(true)
bool(true)
-- Iteration 9 --
bool(true)
bool(true)
-- Iteration 10 --
bool(true)
bool(true)
-- Iteration 11 --
bool(true)
bool(true)

*** Testing is_writable() on directory without write permission ***
bool(false)
bool(false)

*** Testing miscelleneous input for is_writable() function ***
-- Iteration 1 --
bool(true)
bool(true)
-- Iteration 2 --
bool(true)
bool(true)
-- Iteration 3 --
bool(true)
bool(true)
-- Iteration 4 --
bool(false)
bool(false)
-- Iteration 5 --
bool(true)
bool(true)
-- Iteration 6 --
bool(true)
bool(true)
-- Iteration 7 --
bool(false)
bool(false)
-- Iteration 8 --
bool(true)
bool(true)
-- Iteration 9 --
bool(false)
bool(false)
-- Iteration 10 --
bool(false)
bool(false)

*** Testing is_writable() on miscelleneous filenames ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: is_writable() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: is_writeable() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
bool(false)
bool(false)

Warning: is_writable() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL

Warning: is_writeable() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL
Done
--UEXPECTF--
*** Testing is_writable(): usage variations ***
-- Iteration 1 --
bool(true)
bool(true)
-- Iteration 2 --
bool(false)
bool(false)
-- Iteration 3 --
bool(true)
bool(true)
-- Iteration 4 --
bool(true)
bool(true)
-- Iteration 5 --
bool(false)
bool(false)
-- Iteration 6 --
bool(false)
bool(false)
-- Iteration 7 --
bool(true)
bool(true)
-- Iteration 8 --
bool(true)
bool(true)
-- Iteration 9 --
bool(true)
bool(true)
-- Iteration 10 --
bool(true)
bool(true)
-- Iteration 11 --
bool(true)
bool(true)

*** Testing is_writable() on directory without write permission ***
bool(false)
bool(false)

*** Testing miscelleneous input for is_writable() function ***
-- Iteration 1 --
bool(true)
bool(true)
-- Iteration 2 --
bool(true)
bool(true)
-- Iteration 3 --
bool(true)
bool(true)
-- Iteration 4 --
bool(false)
bool(false)
-- Iteration 5 --
bool(true)
bool(true)
-- Iteration 6 --
bool(true)
bool(true)
-- Iteration 7 --
bool(false)
bool(false)
-- Iteration 8 --
bool(true)
bool(true)
-- Iteration 9 --
bool(false)
bool(false)
-- Iteration 10 --
bool(false)
bool(false)

*** Testing is_writable() on miscelleneous filenames ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: is_writable() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: is_writeable() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
bool(false)
bool(false)

Warning: is_writable() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL

Warning: is_writeable() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL
Done
