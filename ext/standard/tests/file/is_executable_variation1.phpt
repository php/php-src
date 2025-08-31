--TEST--
Test is_executable() function: usage variations - diff. path notations
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not for windows');
}
?>
--FILE--
<?php
/* test is_executable() with file having different filepath notation */

require __DIR__.'/file.inc';
echo "*** Testing is_executable(): usage variations ***\n";

$file_path = __DIR__;
mkdir("$file_path/is_executable_variation1");

// create a new temporary file
$fp = fopen("$file_path/is_executable_variation1/bar.tmp", "w");
fclose($fp);

/* array of files checked to see if they are executable files
   using is_executable() function */
$files_arr = array(
  "$file_path/is_executable_variation1/bar.tmp",

  /* Testing a file with trailing slash */
  "$file_path/is_executable_variation1/bar.tmp/",

  /* Testing file with double slashes */
  "$file_path/is_executable_variation1//bar.tmp",
  "$file_path/is_executable_variation1/*.tmp",
  "$file_path/is_executable_variation1/b*.tmp",

  /* Testing Binary safe */
  "$file_path/is_executable_variation1".chr(0)."bar.temp",
  "$file_path".chr(0)."is_executable_variation1/bar.temp",
  "$file_path/is_executable_variation1x000/",

  /* Testing directories */
  ".",  // current directory, exp: bool(true)
  "$file_path/is_executable_variation1"  // temp directory, exp: bool(true)
);
$counter = 1;
/* loop through to test each element in the above array
   is an executable file */
foreach($files_arr as $file) {
  echo "-- Iteration $counter --\n";
  try {
    var_dump( is_executable($file) );
  } catch (Error $e) {
    echo $e->getMessage(), "\n";
  }
  $counter++;
  clearstatcache();
}

echo "Done\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/is_executable_variation1/bar.tmp");
rmdir(__DIR__."/is_executable_variation1/");
?>
--EXPECT--
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
bool(false)
-- Iteration 7 --
bool(false)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
bool(true)
-- Iteration 10 --
bool(true)
Done
