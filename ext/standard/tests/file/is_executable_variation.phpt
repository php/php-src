--TEST--
Test is_executable() function: usage variations
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
/* Prototype: bool is_executable ( string $filename );
   Description: Tells whether the filename is executable
*/

require dirname(__FILE__).'/file.inc';
echo "*** Testing is_executable(): usage variations ***\n";

$file_path = dirname(__FILE__);
mkdir("$file_path/is_executable");

// create a new temporary file
$fp = fopen("$file_path/is_executable/bar.tmp", "w");
fclose($fp);

/* array of files checked to see if they are executable files
   using is_executable() function */
$files_arr = array(
  "$file_path/is_executable/bar.tmp",

  /* Testing a file with trailing slash */
  "$file_path/is_executable/bar.tmp/",

  /* Testing file with double slashes */
  "$file_path/is_executable//bar.tmp",
  "$file_path/is_executable/*.tmp",
  "$file_path/is_executable/b*.tmp", 

  /* Testing Binary safe */
  "$file_path/is_executable".chr(0)."bar.temp",
  "$file_path".chr(0)."is_executable/bar.temp",
  "$file_path/is_executablex000/",
  
  /* Testing directories */
  ".",  // current directory, exp: bool(true)
  "$file_path/is_executable"  // temp directory, exp: bool(true)
);
$counter = 1;
/* loop through to test each element in the above array 
   is an executable file */
foreach($files_arr as $file) {
  echo "-- Iteration $counter --\n";
  var_dump( is_executable($file) );
  $counter++;
  clearstatcache();
}

echo "\n*** Testing is_executable() on directory without execute permission ***\n";
chmod("$file_path/is_executable", 0444);
var_dump( is_executable("$file_path/is_executable") );  // exp: bool(false)
chmod("$file_path/is_executable", 0777);  // chmod to enable deletion of directory

echo "\n*** Testing miscelleneous input for is_executable() function ***\n";
$name_prefix = "is_executable";
create_files(dirname(__FILE__), 1, "numeric", 0755, 1, "w", $name_prefix, 1);
create_files(dirname(__FILE__), 1, "text", 0755, 1, "w", $name_prefix, 2);
create_files(dirname(__FILE__), 1, "empty", 0755, 1, "w", $name_prefix, 3);
create_files(dirname(__FILE__), 1, "numeric", 0755, 1, "w", $name_prefix, 4);
create_files(dirname(__FILE__), 1, "text", 0222, 1, "w", $name_prefix, 5);
create_files(dirname(__FILE__), 1, "numeric", 0711, 1, "w", $name_prefix, 6);
create_files(dirname(__FILE__), 1, "text", 0714, 1, "w", $name_prefix, 7);
create_files(dirname(__FILE__), 1, "numeric", 0744, 1, "w", $name_prefix, 8);
create_files(dirname(__FILE__), 1, "text", 0421, 1, "w", $name_prefix, 9);
create_files(dirname(__FILE__), 1, "text", 0712, 1, "w", $name_prefix, 10);

$files = array (
  "$file_path/is_executable1.tmp",
  "$file_path/is_executable2.tmp",
  "$file_path/is_executable3.tmp",
  "$file_path/is_executable4.tmp",
  "$file_path/is_executable5.tmp",
  "$file_path/is_executable6.tmp",
  "$file_path/is_executable7.tmp",
  "$file_path/is_executable8.tmp",
  "$file_path/is_executable9.tmp",
  "$file_path/is_executable10.tmp",
);
$counter = 1;
/* loop through to test each element in the above array
   is an executable file */
foreach($files as $file) {
  echo "-- Iteration $counter --\n";
  var_dump( is_executable($file) );
  $counter++;
  clearstatcache();
}

// change all file's permissions to 777 before deleting
change_file_perms($file_path, 10, 0777, $name_prefix);
delete_files($file_path, 10, $name_prefix);

$file_handle = fopen(__FILE__, "r");
unset($file_handle);

echo "\n*** Testing is_executable() on invalid files ***\n";
$invalid_files = array(
  0,
  1234,
  -2.34555,
  "string",
  TRUE,
  FALSE,
  NULL,
  @array(),
  @$file_handle
);
/* loop through to test each element in the above array 
   is an executable file */
foreach( $invalid_files as $invalid_file ) {
  var_dump( is_executable($invalid_file) );
  clearstatcache();
}

echo "Done\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/is_executable/bar.tmp");
rmdir(dirname(__FILE__)."/is_executable/");
?>
--EXPECTF--
*** Testing is_executable(): usage variations ***
-- Iteration 1 --
bool(false)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(false)
-- Iteration 4 --
bool(false)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
bool(true)
-- Iteration 10 --
bool(true)

*** Testing is_executable() on directory without execute permission ***
bool(false)

*** Testing miscelleneous input for is_executable() function ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(true)
-- Iteration 9 --
bool(false)
-- Iteration 10 --
bool(true)

*** Testing is_executable() on invalid files ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: is_executable() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
bool(false)
Done
--UEXPECTF--
*** Testing is_executable(): usage variations ***
-- Iteration 1 --
bool(false)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(false)
-- Iteration 4 --
bool(false)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
bool(true)
-- Iteration 10 --
bool(true)

*** Testing is_executable() on directory without execute permission ***
bool(false)

*** Testing miscelleneous input for is_executable() function ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(true)
-- Iteration 9 --
bool(false)
-- Iteration 10 --
bool(true)

*** Testing is_executable() on invalid files ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: is_executable() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
bool(false)
Done
