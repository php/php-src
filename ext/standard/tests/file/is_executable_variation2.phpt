--TEST--
Test is_executable() function: usage variations - file/dir with diff. perms
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not for windows');
}
// Skip if being run by root (files are always readable, writeable and executable)
$filename = dirname(__FILE__)."/is_executable_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
        unlink ($filename);
        die('skip cannot be run as root');
}

unlink($filename);
?>
--FILE--
<?php
/* Prototype: bool is_executable ( string $filename );
   Description: Tells whether the filename is executable
*/

/* test is_executable() with file/dir having different permissions */

require dirname(__FILE__).'/file.inc';
echo "*** Testing is_executable(): usage variations ***\n";

$file_path = dirname(__FILE__);
mkdir("$file_path/is_executable_variation2");

echo "\n*** Testing is_executable() on directory without execute permission ***\n";
chmod("$file_path/is_executable_variation2", 0444);
var_dump( is_executable("$file_path/is_executable_variation2") );  // exp: bool(false)
chmod("$file_path/is_executable_variation2", 0777);  // chmod to enable deletion of directory

echo "\n*** Testing miscelleneous input for is_executable() function ***\n";
$name_prefix = "is_executable_variation2";
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
  "$file_path/is_executable_variation21.tmp",
  "$file_path/is_executable_variation22.tmp",
  "$file_path/is_executable_variation23.tmp",
  "$file_path/is_executable_variation24.tmp",
  "$file_path/is_executable_variation25.tmp",
  "$file_path/is_executable_variation26.tmp",
  "$file_path/is_executable_variation27.tmp",
  "$file_path/is_executable_variation28.tmp",
  "$file_path/is_executable_variation29.tmp",
  "$file_path/is_executable_variation210.tmp",
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

echo "Done\n";
?>
--CLEAN--
<?php
rmdir(dirname(__FILE__)."/is_executable_variation2/");
?>
--EXPECTF--
*** Testing is_executable(): usage variations ***

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
Done
