--TEST--
Testing disk_total_space() functions : Usage Variations.
--CONFLICTS--
disk_total_space
--FILE--
<?php
$file_path = __DIR__;

echo "*** Testing with a directory ***\n";
var_dump( disk_total_space($file_path."/..") );

echo "\nTesting for the return type ***\n";
$return_value = disk_total_space($file_path);
var_dump( is_float($return_value) );

echo "\n*** Testing with different directory combinations ***";
$dir = "/disk_total_space";

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
    var_dump( disk_total_space( $dir1 ) );
  } catch (Error $e) {
    echo $e->getMessage(), "\n";
  }
  $count++;
}

echo "*** Testing with Binary Input ***\n";
var_dump( disk_total_space(b"$file_path") );

echo"\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
rmdir($file_path."/disk_total_space");
?>
--EXPECTF--
*** Testing with a directory ***
float(%f)

Testing for the return type ***
bool(true)

*** Testing with different directory combinations ***
-- Iteration 1 --
float(%f)

-- Iteration 2 --
float(%f)

-- Iteration 3 --
float(%f)

-- Iteration 4 --
float(%f)

-- Iteration 5 --
float(%f)

-- Iteration 6 --
float(%f)

-- Iteration 7 --
float(%f)

-- Iteration 8 --
float(%f)

-- Iteration 9 --
disk_total_space(): Argument #1 ($directory) must not contain any null bytes

-- Iteration 10 --
disk_total_space(): Argument #1 ($directory) must not contain any null bytes

-- Iteration 11 --
disk_total_space(): Argument #1 ($directory) must not contain any null bytes

-- Iteration 12 --
disk_total_space(): Argument #1 ($directory) must not contain any null bytes
*** Testing with Binary Input ***
float(%s)

--- Done ---
