--TEST--
Test is_readable() function: usage variations
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
/* Prototype: bool is_readable ( string $filename );
   Description: Tells whether the filename is readable.
*/

require dirname(__FILE__).'/file.inc';
echo "*** Testing is_readable(): usage variations ***\n";

$file_path = dirname(__FILE__);
mkdir("$file_path/is_readable");

//create a temporary file
$fp = fopen("$file_path/is_readable/bar.tmp", "w");
fclose($fp);

/* array of files to be tested if they are readable by using
   is_readable() function */
$files_arr = array(
  "$file_path/is_readable/bar.tmp",

  /* Testing a file trailing slash */
  "$file_path/is_readable/bar.tmp/",

  /* Testing file with double slashes */
  "$file_path/is_readable//bar.tmp",
  "$file_path//is_readable//bar.tmp",
  "$file_path/is_readable/*.tmp",
  "$file_path/is_readable/b*.tmp", 

  /* Testing Binary safe */
  "$file_path/is_readable".chr(47)."bar.tmp",
  "$file_path".chr(47)."is_readable/bar.tmp",
  b"$file_path/is_readable/bar.tmp",
  
  /* Testing directories */
  ".",  // current directory, exp: bool(true)
  "$file_path/is_readable"  // temp directory, exp: bool(true)
);
$counter = 1;
/* loop through to test each element in the above array 
   is a writable file */
foreach($files_arr as $file) {
  echo "-- Iteration $counter --\n";
  var_dump( is_readable($file) );
  $counter++;
  clearstatcache();
}

echo "\n*** Testing is_readable() on directory without read permission ***\n";
chmod("$file_path/is_readable", 0001);
var_dump( is_readable("$file_path/is_readable") );  // exp: bool(false)
chmod("$file_path/is_readable", 0777);  // chmod to enable deletion of directory

echo "\n*** Testing miscelleneous input for is_readable() function ***\n";
$name_prefix = "is_readable";
create_files(dirname(__FILE__), 1, "numeric", 0755, 1, "w", $name_prefix, 1);
create_files(dirname(__FILE__), 1, "text", 0755, 1, "w", $name_prefix, 2);
create_files(dirname(__FILE__), 1, "empty", 0755, 1, "w", $name_prefix, 3);
create_files(dirname(__FILE__), 1, "numeric", 0555, 1, "w", $name_prefix, 4);
create_files(dirname(__FILE__), 1, "text", 0222, 1, "w", $name_prefix, 5);
create_files(dirname(__FILE__), 1, "numeric", 0711, 1, "w", $name_prefix, 6);
create_files(dirname(__FILE__), 1, "text", 0411, 1, "w", $name_prefix, 7);
create_files(dirname(__FILE__), 1, "numeric", 0444, 1, "w", $name_prefix, 8);
create_files(dirname(__FILE__), 1, "text", 0421, 1, "w", $name_prefix, 9);
create_files(dirname(__FILE__), 1, "text", 0422, 1, "w", $name_prefix, 10);

$files = array (
  "$file_path/is_readable1.tmp",
  "$file_path/is_readable2.tmp",
  "$file_path/is_readable3.tmp",
  "$file_path/is_readable4.tmp",
  "$file_path/is_readable5.tmp",
  "$file_path/is_readable6.tmp",
  "$file_path/is_readable7.tmp",
  "$file_path/is_readable8.tmp",
  "$file_path/is_readable9.tmp",
  "$file_path/is_readable10.tmp"
);
$counter = 1;
/* loop through to test each element in the above array
   is a readable file */
foreach($files as $file) {
  echo "-- Iteration $counter --\n";
  var_dump( is_readable($file) );
  $counter++;
  clearstatcache();
}

// change all file's permissions to 777 before deleting
change_file_perms($file_path, 10, 0777, $name_prefix);
delete_files($file_path, 10, $name_prefix);

$file_handle = fopen(__FILE__, "r");
unset($file_handle);

echo "\n*** Testing is_readable() on miscelleneous filenames ***\n";
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
   is a readable file */
foreach( $misc_files as $misc_file ) {
  var_dump( is_readable($misc_file) );
  clearstatcache();
}

echo "Done\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/is_readable/bar.tmp");
rmdir(dirname(__FILE__)."/is_readable/");
?>
--EXPECTF--
*** Testing is_readable(): usage variations ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(false)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(true)
-- Iteration 9 --
bool(true)
-- Iteration 10 --
bool(true)
-- Iteration 11 --
bool(true)

*** Testing is_readable() on directory without read permission ***
bool(false)

*** Testing miscelleneous input for is_readable() function ***
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
bool(true)
-- Iteration 10 --
bool(true)

*** Testing is_readable() on miscelleneous filenames ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: is_readable() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
bool(false)

Warning: is_readable() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL
Done
--UEXPECTF--
*** Testing is_readable(): usage variations ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(false)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(true)
-- Iteration 9 --
bool(true)
-- Iteration 10 --
bool(true)
-- Iteration 11 --
bool(true)

*** Testing is_readable() on directory without read permission ***
bool(false)

*** Testing miscelleneous input for is_readable() function ***
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
bool(true)
-- Iteration 10 --
bool(true)

*** Testing is_readable() on miscelleneous filenames ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: is_readable() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
bool(false)

Warning: is_readable() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL
Done
