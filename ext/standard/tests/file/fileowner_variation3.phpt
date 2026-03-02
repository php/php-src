--TEST--
Test fileowner() function: usage variations - diff. path notations
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php

/* Passing file names with different notations, using slashes, wild-card chars */

$file_path = __DIR__;

echo "*** Testing fileowner() with different notations of file names ***\n";
$dir_name = $file_path."/fileowner_variation3";
mkdir($dir_name);
$file_handle = fopen($dir_name."/fileowner_variation3.tmp", "w");
fclose($file_handle);

$files_arr = array(
  "/fileowner_variation3/fileowner_variation3.tmp",

  /* Testing a file trailing slash */
  "/fileowner_variation3/fileowner_variation3.tmp/",

  /* Testing file with double slashes */
  "/fileowner_variation3//fileowner_variation3.tmp",
  "//fileowner_variation3//fileowner_variation3.tmp",
  "/fileowner_variation3/*.tmp",
  "fileowner_variation3/fileowner*.tmp",

  /* Testing Binary safe */
  "/fileowner_variation3/fileowner_variation3.tmp".chr(0),
  "/fileowner_variation3/fileowner_variation3.tmp\0"
);

$count = 1;
/* loop through to test each element in the above array */
foreach($files_arr as $file) {
  echo "- Iteration $count -\n";
  try {
    var_dump( fileowner( $file_path."/".$file ) );
  } catch (Error $e) {
    echo $e->getMessage(), "\n";
  }
  clearstatcache();
  $count++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$dir_name = $file_path."/fileowner_variation3";
unlink($dir_name."/fileowner_variation3.tmp");
rmdir($dir_name);
?>
--EXPECTF--
*** Testing fileowner() with different notations of file names ***
- Iteration 1 -
int(%d)
- Iteration 2 -

Warning: fileowner(): stat failed for %s//fileowner_variation3/fileowner_variation3.tmp/ in %s on line %d
bool(false)
- Iteration 3 -
int(%d)
- Iteration 4 -
int(%d)
- Iteration 5 -

Warning: fileowner(): stat failed for %s//fileowner_variation3/*.tmp in %s on line %d
bool(false)
- Iteration 6 -

Warning: fileowner(): stat failed for %s/fileowner_variation3/fileowner*.tmp in %s on line %d
bool(false)
- Iteration 7 -

Warning: fileowner(): Filename contains null byte in %s on line %d
bool(false)
- Iteration 8 -

Warning: fileowner(): Filename contains null byte in %s on line %d
bool(false)

*** Done ***
