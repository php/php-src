--TEST--
Test disk_free_space and its alias diskfreespace() functions : Usage Variations
--FILE--
<?php
/*
 *  Prototype: float disk_free_space( string directory )
 *  Description: Given a string containing a directory, this function
 *               will return the number of bytes available on the corresponding
 *               filesystem or disk partition
 */

$file_path = __DIR__;

echo "*** Testing with a directory ***\n";
var_dump( disk_free_space($file_path."/..") );
var_dump( diskfreespace($file_path."/..") );

echo "\nTesting for the return type ***\n";
$return_value = disk_free_space($file_path);
var_dump( is_float($return_value) );

echo "\n*** Testing with different directory combinations ***";
$dir = "/disk_free_space_variation";
mkdir($file_path.$dir);

$dirs_arr = array(
  ".",
  $file_path.$dir,
  $file_path."/.".$dir,

  /* Testing a file trailing slash */
  $file_path."".$dir."/",
  $file_path."/.".$dir."/",

  /* Testing file with double trailing slashes */
  $file_path.$dir."//",
  $file_path."/.".$dir."//",
  $file_path."/./".$dir."//",

  /* Testing Binary safe */
  $file_path.$dir.chr(0),
  $file_path."/.".$dir.chr(0),
  ".".chr(0).$file_path.$dir,
  ".".chr(0).$file_path.$dir.chr(0)
);

$count = 1;
/* loop through to test each element the above array */
foreach($dirs_arr as $dir1) {
  echo "\n-- Iteration $count --\n";
  try {
    var_dump( disk_free_space( $dir1 ) );
  } catch (TypeError $e) {
    echo $e->getMessage(), "\n";
  }
  try {
    var_dump( diskfreespace( $dir1 ) );
  } catch (TypeError $e) {
    echo $e->getMessage(), "\n";
  }
  $count++;
}

echo"\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
rmdir($file_path."/disk_free_space_variation");
?>
--EXPECTF--
*** Testing with a directory ***
float(%d)
float(%d)

Testing for the return type ***
bool(true)

*** Testing with different directory combinations ***
-- Iteration 1 --
float(%d)
float(%d)

-- Iteration 2 --
float(%d)
float(%d)

-- Iteration 3 --
float(%d)
float(%d)

-- Iteration 4 --
float(%d)
float(%d)

-- Iteration 5 --
float(%d)
float(%d)

-- Iteration 6 --
float(%d)
float(%d)

-- Iteration 7 --
float(%d)
float(%d)

-- Iteration 8 --
float(%d)
float(%d)

-- Iteration 9 --
disk_free_space(): Argument #1 ($directory) must be a valid path, string given
diskfreespace(): Argument #1 ($directory) must be a valid path, string given

-- Iteration 10 --
disk_free_space(): Argument #1 ($directory) must be a valid path, string given
diskfreespace(): Argument #1 ($directory) must be a valid path, string given

-- Iteration 11 --
disk_free_space(): Argument #1 ($directory) must be a valid path, string given
diskfreespace(): Argument #1 ($directory) must be a valid path, string given

-- Iteration 12 --
disk_free_space(): Argument #1 ($directory) must be a valid path, string given
diskfreespace(): Argument #1 ($directory) must be a valid path, string given

--- Done ---
