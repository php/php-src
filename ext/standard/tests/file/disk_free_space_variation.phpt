--TEST--
Test disk_free_space and its alias diskfreespace() functions : usage variations
--FILE--
<?php
/*
 *  Prototype: float disk_free_space( string directory )
 *  Description: Given a string containing a directory, this function 
 *               will return the number of bytes available on the corresponding
 *               filesystem or disk partition
 */

$file_path = dirname(__FILE__);

echo "*** Testing disk_free_space() function with a directory ***\n";
var_dump( disk_free_space($file_path."/..") ); 
var_dump( diskfreespace($file_path."/..") ); 

echo "\n*** Testing for the return type ***\n";
$return_value = disk_free_space($file_path); 
var_dump( is_float($return_value) );

echo "\n*** Testing disk_free_space() function with different styles of file and directory ***";

$dir = "/disk_free_space";
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
foreach($dirs_arr as $dir) {
  echo "\n-- Iteration $count --\n";
  var_dump( disk_free_space( $dir ) );
  var_dump( diskfreespace( $dir ) );
  $count++;
}

echo"\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__)."/disk_free_space";
rmdir($file_path);
?>

--EXPECTF--
*** Testing disk_free_space() function with a directory ***
float(%f)
float(%f)

*** Testing for the return type ***
bool(true)

*** Testing disk_free_space() function with different styles of file and directory ***
-- Iteration 1 --
float(%f)
float(%f)

-- Iteration 2 --
float(%f)
float(%f)

-- Iteration 3 --
float(%f)
float(%f)

-- Iteration 4 --
float(%f)
float(%f)

-- Iteration 5 --
float(%f)
float(%f)

-- Iteration 6 --
float(%f)
float(%f)

-- Iteration 7 --
float(%f)
float(%f)

-- Iteration 8 --
float(%f)
float(%f)

-- Iteration 9 --
float(%f)
float(%f)

-- Iteration 10 --
float(%f)
float(%f)

-- Iteration 11 --
float(%f)
float(%f)

-- Iteration 12 --
float(%f)
float(%f)

--- Done ---
