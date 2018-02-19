--TEST--
Test is_writable() and its alias is_writeable() function: usage variations - file/dir with diff. perms
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only on LINUX');
}
// Skip if being run by root
$filename = dirname(__FILE__)."/is_readable_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
        unlink ($filename);
        die('skip cannot be run as root');
}

unlink ($filename);
?>
--FILE--
<?php
/* Prototype: bool is_writable ( string $filename );
   Description: Tells whether the filename is writable.

   is_writeable() is an alias of is_writable()
*/

/* test is_executable() with file/dir having different permissions */

require dirname(__FILE__).'/file.inc';
echo "*** Testing is_writable(): usage variations ***\n";

$file_path = dirname(__FILE__);
mkdir("$file_path/is_writable_variation2");

echo "\n*** Testing is_writable() on directory without write permission ***\n";
chmod("$file_path/is_writable_variation2", 0004);
var_dump( is_writable("$file_path/is_writable_variation2") );  // exp: bool(false)
var_dump( is_writeable("$file_path/is_writable_variation2") );  // exp: bool(false)
chmod("$file_path/is_writable_variation2", 0777);  // chmod to enable deletion of directory

echo "\n*** Testing miscelleneous input for is_writable() function ***\n";
$name_prefix = "is_writable_variation2";
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
  "$file_path/is_writable_variation21.tmp",
  "$file_path/is_writable_variation22.tmp",
  "$file_path/is_writable_variation23.tmp",
  "$file_path/is_writable_variation24.tmp",
  "$file_path/is_writable_variation25.tmp",
  "$file_path/is_writable_variation26.tmp",
  "$file_path/is_writable_variation27.tmp",
  "$file_path/is_writable_variation28.tmp",
  "$file_path/is_writable_variation29.tmp",
  "$file_path/is_writable_variation210.tmp"
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

echo "Done\n";
?>
--CLEAN--
<?php
rmdir(dirname(__FILE__)."/is_writable_variation2/");
?>
--EXPECT--
*** Testing is_writable(): usage variations ***

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
Done
