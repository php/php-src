--TEST--
Test is_readable() function: usage variations - file/dir with diff. perms
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not for windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/* Prototype: bool is_readable ( string $filename );
   Description: Tells whether the filename is readable.
*/

/* test is_executable() with file/dir having different permissions */

require __DIR__.'/file.inc';
echo "*** Testing is_readable(): usage variations ***\n";

$file_path = __DIR__;
mkdir("$file_path/is_readable_variation2");

echo "\n*** Testing is_readable() on directory without read permission ***\n";
chmod("$file_path/is_readable_variation2", 0001);
var_dump( is_readable("$file_path/is_readable_variation2") );  // exp: bool(false)
chmod("$file_path/is_readable_variation2", 0777);  // chmod to enable deletion of directory

echo "\n*** Testing miscelleneous input for is_readable() function ***\n";
$name_prefix = "is_readable_variation2";
create_files(__DIR__, 1, "numeric", 0755, 1, "w", $name_prefix, 1);
create_files(__DIR__, 1, "text", 0755, 1, "w", $name_prefix, 2);
create_files(__DIR__, 1, "empty", 0755, 1, "w", $name_prefix, 3);
create_files(__DIR__, 1, "numeric", 0555, 1, "w", $name_prefix, 4);
create_files(__DIR__, 1, "text", 0222, 1, "w", $name_prefix, 5);
create_files(__DIR__, 1, "numeric", 0711, 1, "w", $name_prefix, 6);
create_files(__DIR__, 1, "text", 0411, 1, "w", $name_prefix, 7);
create_files(__DIR__, 1, "numeric", 0444, 1, "w", $name_prefix, 8);
create_files(__DIR__, 1, "text", 0421, 1, "w", $name_prefix, 9);
create_files(__DIR__, 1, "text", 0422, 1, "w", $name_prefix, 10);

$files = array (
  "$file_path/is_readable_variation21.tmp",
  "$file_path/is_readable_variation22.tmp",
  "$file_path/is_readable_variation23.tmp",
  "$file_path/is_readable_variation24.tmp",
  "$file_path/is_readable_variation25.tmp",
  "$file_path/is_readable_variation26.tmp",
  "$file_path/is_readable_variation27.tmp",
  "$file_path/is_readable_variation28.tmp",
  "$file_path/is_readable_variation29.tmp",
  "$file_path/is_readable_variation210.tmp"
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

echo "Done\n";
?>
--CLEAN--
<?php
rmdir(__DIR__."/is_readable_variation2/");
?>
--EXPECT--
*** Testing is_readable(): usage variations ***

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
Done
