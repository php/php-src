--TEST--
Test disk_total_space() functions : usage variations
--FILE--
<?php
/*
 *  Prototype: float disk_total_space( string directory )
 *  Description: given a string containing a directory, this function 
 *               will return the total number of bytes on the corresponding 
 *               filesystem or disk partition.
 */

$file_path = dirname(__FILE__);

echo "*** Testing with a directory ***\n";
var_dump( disk_total_space($file_path."/..") ); 

echo "\nTesting for the return type ***\n";
$return_value = disk_total_space($file_path); 
var_dump( is_float($return_value) );

echo "\n*** Testing disk_total_space() function with different directory combinations ***";

$dir = "/disk_total_space";
mkdir($file_path.$dir);

$dirs_arr = array(
  ".",
  $file_path.$dir,
  $file_path."/.".$dir,

  /* Testing a file trailing slash */
  $file_path.$dir."/",
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
  var_dump( disk_total_space( $dir1 ) );
  $count++;
}

echo"\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
$dir = "/disk_total_space";
rmdir($file_path.$dir);
?>
--EXPECTF--
*** Testing with a directory ***
float(%f)

Testing for the return type ***
bool(true)

*** Testing disk_total_space() function with different directory combinations ***
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
float(%f)

-- Iteration 10 --
float(%f)

-- Iteration 11 --
float(%f)

-- Iteration 12 --
float(%f)

--- Done ---
